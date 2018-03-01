#include<GL/glew.h>
#include"PBFSolverGPU.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../../Graphics/AABB/AABB.h"
#include<list>
#include<iostream>
#include<GL/glew.h>
#include<glm/gtc/random.hpp>

PBFSolverGPU::PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    Shader computeShader(GL_COMPUTE_SHADER,"Resources/Simulation/simulation.comp");
    if(!computeShader.compile())
    {
        std::cout<<computeShader.compileLog()<<std::endl;
    }
    computeProgram = new ShaderProgram();
    computeProgram->attachShader(computeShader);
    if(!computeProgram->link())
    {
        std::cout<<computeProgram->linkLog()<<std::endl;
    }
    computeProgram->bind();
}

PBFSolverGPU::PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    this->constraints = constraints;
}

void PBFSolverGPU::init()
{
}

AABB PBFSolverGPU::solve(const AABB& aabb)
{
    glm::ivec3 dimSize;
    const glm::vec3 ext = aabb.getExtent();
    const glm::vec3 min = aabb.min;
    dimSize.x = glm::max(std::floor(ext.x/kernelSupport+1),1.0f);
    dimSize.y = glm::max(std::floor(ext.y/kernelSupport+1),1.0f);
    dimSize.z = glm::max(std::floor(ext.z/kernelSupport+1),1.0f);
    unsigned int elems = dimSize.x*dimSize.y*dimSize.z;

    //Setup Compute Shader
    std::vector<unsigned int> bufData((elems+(2048-elems%2048)));

    unsigned int histBuf;
    unsigned int ofsBuf;
    unsigned int backBuf;
    unsigned int sumsBuf;
    unsigned int incrBuf;

    glGenBuffers(1,&backBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,backBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(Particle)*particles.size(),NULL,GL_DYNAMIC_DRAW);

    glGenBuffers(1,&histBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,histBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*(elems+(1024-elems%1024)),bufData.data(),GL_DYNAMIC_DRAW);

    glGenBuffers(1,&ofsBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,ofsBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*(elems+(1024-elems%1024)),bufData.data(),GL_DYNAMIC_DRAW);

    glGenBuffers(1,&sumsBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,sumsBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*1024,bufData.data(),GL_DYNAMIC_DRAW);

    glGenBuffers(1,&incrBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,5,incrBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*1024,bufData.data(),GL_DYNAMIC_DRAW);


    computeProgram->bind();
    computeProgram->uploadScalar("timestep",timestep);
    computeProgram->uploadUnsignedInt("iterations",iterations);
    computeProgram->uploadScalar("kernelSupport",kernelSupport);
    computeProgram->uploadScalar("restDensity",restDensity);
    computeProgram->uploadScalar("artVisc",artVisc);
    computeProgram->uploadScalar("artVort",artVort);
    computeProgram->uploadScalar("cfmRegularization",cfmRegularization);
    computeProgram->uploadScalar("corrConst",corrConst);
    computeProgram->uploadScalar("corrDist",corrDist);
    computeProgram->uploadScalar("corrExp",corrExp);
    computeProgram->uploadVec3("minOfs",min);
    computeProgram->uploadIVec3("dimSize",dimSize);
    computeProgram->uploadScalar("particleSize",particleSize);
    computeProgram->uploadUnsignedInt("nParticles",particles.size());
    computeProgram->uploadUnsignedInt("nBuckets",elems);
    computeProgram->uploadScalar("aabbMaxX",bounds.max.x);
    computeProgram->uploadScalar("aabbMaxY",bounds.max.y);
    computeProgram->uploadScalar("aabbMaxZ",bounds.max.z);
    computeProgram->uploadScalar("aabbMinX",bounds.min.x);
    computeProgram->uploadScalar("aabbMinY",bounds.min.y);
    computeProgram->uploadScalar("aabbMinZ",bounds.min.z);

    glm::ivec3 maxSize = computeProgram->getMaxWorkGroups();

    unsigned int workGroupSize = 1024;
    unsigned int workGroups = std::ceil(particles.size()/((float)workGroupSize));

    GLsync syncObj;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",0);
    computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",1);
    computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);


    computeProgram->uploadUnsignedInt("taskId",2);
    computeProgram->dispatch(ceil(elems/1024.0),1,1,512,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",10);
    computeProgram->dispatch(1,1,1,ceil(elems/1024.0),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);


    computeProgram->uploadUnsignedInt("taskId",11);
    computeProgram->dispatch(ceil(elems/1024.0),1,1,1024,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",3);
    computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    for(unsigned i=0;i<iterations;i++)
    {
        computeProgram->uploadUnsignedInt("taskId",4);
        computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",5);
        computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",6);
        computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",7);
        computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

    }

    computeProgram->uploadUnsignedInt("taskId",8);
    computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",9);
    computeProgram->dispatch(workGroups,1,1,workGroupSize,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);
    //glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,5,0);


    glDeleteBuffers(1,&backBuf);
    glDeleteBuffers(1,&histBuf);
    glDeleteBuffers(1,&ofsBuf);
    glDeleteBuffers(1,&sumsBuf);
    glDeleteBuffers(1,&incrBuf);

}

void PBFSolverGPU::setAABBMinX(float val)
{
    bounds.min.x = val;
}

void PBFSolverGPU::setAABBMaxX(float val)
{
    bounds.max.x = val;
}

void PBFSolverGPU::setAABBMinY(float val)
{
    bounds.min.z = val;
}

void PBFSolverGPU::setAABBMaxY(float val)
{
    bounds.max.z = val;
}

void PBFSolverGPU::setDensityKernel(int id)
{

}

void PBFSolverGPU::setGradKernel(int id)
{

}

void PBFSolverGPU::setViscKernel(int id)
{

}

void PBFSolverGPU::setNumIterations(unsigned int iterations)
{
    this->iterations = iterations;
}

int PBFSolverGPU::getNumIterations()
{
    return iterations;
}

void PBFSolverGPU::setTimestep(float timestep)
{
    this->timestep = timestep;
}

float PBFSolverGPU::getTimestep()
{
    return timestep;
}

void PBFSolverGPU::setKernelSupport(float support)
{
    this->kernelSupport = support;
}

float PBFSolverGPU::getKernelSupport()
{
    return kernelSupport;
}

void PBFSolverGPU::setRestDensity(float density)
{
    this->restDensity = density;
}

float PBFSolverGPU::getRestDensity()
{
    return restDensity;
}

void PBFSolverGPU::setArtVisc(float artVisc)
{
    this->artVisc = artVisc;
}

float PBFSolverGPU::getArtVisc()
{
    return artVisc;
}

void PBFSolverGPU::setArtVort(float artVort)
{
    this->artVort = artVort;
}

float PBFSolverGPU::getArtVort()
{
    return artVort;
}

void PBFSolverGPU::setCfmRegularization(float cfm)
{
    this->cfmRegularization = cfm;
}

float PBFSolverGPU::getCfmRegularization()
{
    return cfmRegularization;
}

void PBFSolverGPU::setCorrConst(float corrConst)
{
    this->corrConst = corrConst;
}

float PBFSolverGPU::getCorrConst()
{
    return corrConst;
}

void PBFSolverGPU::setCorrDist(float corrDist)
{
    this->corrDist = corrDist;
}

float PBFSolverGPU::getCorrDist()
{
    return corrDist;
}

void PBFSolverGPU::setCorrExp(float corrExp)
{
    this->corrExp = corrExp;
}

float PBFSolverGPU::getCorrExp()
{
    return corrExp;
}

void PBFSolverGPU::setPartSize(float size)
{
    this->particleSize=size;
}

float PBFSolverGPU::getPartSize()
{
    return particleSize;
}


