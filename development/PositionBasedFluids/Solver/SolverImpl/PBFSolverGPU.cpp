#include<GL/glew.h>
#include"PBFSolverGPU.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../../Graphics/AABB/AABB.h"
#include<list>
#include<iostream>
#include<GL/glew.h>

PBFSolverGPU::PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    Shader computeShader(GL_COMPUTE_SHADER,"Resources/shader.glsl");
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

void PBFSolverGPU::solve()
{
    AABB aabb = AABB(glm::vec3(-1.2,-1.7,-1.2),glm::vec3(1.2,1.7,1.2));
    glm::ivec3 dimSize;
    const glm::vec3 ext = aabb.getExtent();
    const glm::vec3 min = aabb.getCenter()-aabb.min;
    dimSize.x = glm::max(std::ceil(ext.x/kernelSupport),1.0f);
    dimSize.y = glm::max(std::ceil(ext.y/kernelSupport),1.0f);
    dimSize.z = glm::max(std::ceil(ext.z/kernelSupport),1.0f);
    unsigned int elems = dimSize.x*dimSize.y*dimSize.z;

    //Setup Compute Shader
    std::vector<unsigned int> bufData(elems);

    unsigned int histBuf;
    unsigned int ofsBuf;
    unsigned int backBuf;

    glGenBuffers(1,&backBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,backBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(Particle)*particles.size(),NULL,GL_DYNAMIC_DRAW);

    glGenBuffers(1,&histBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,histBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*(elems+1),bufData.data(),GL_DYNAMIC_DRAW);

    glGenBuffers(1,&ofsBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,ofsBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(unsigned int)*elems,bufData.data(),GL_DYNAMIC_DRAW);

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



    GLsync syncObj;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",0);
    computeProgram->dispatch(particles.size(),1,1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",1);
    computeProgram->dispatch(particles.size(),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);


    computeProgram->uploadUnsignedInt("taskId",2);
    computeProgram->dispatch(1,1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);


    computeProgram->uploadUnsignedInt("taskId",3);
    computeProgram->dispatch(particles.size(),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);


    //Iterate
    //glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    for(unsigned i=0;i<iterations;i++)
    {
        computeProgram->uploadUnsignedInt("taskId",4);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",5);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",6);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

        computeProgram->uploadUnsignedInt("taskId",7);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        glClientWaitSync(syncObj,0,1000*1000*1000*2);
        glDeleteSync(syncObj);

    }

    computeProgram->uploadUnsignedInt("taskId",8);
    computeProgram->dispatch(particles.size(),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);

    computeProgram->uploadUnsignedInt("taskId",9);
    computeProgram->dispatch(particles.size(),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    glClientWaitSync(syncObj,0,1000*1000*1000*2);
    glDeleteSync(syncObj);
    //glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,0);


    glDeleteBuffers(1,&backBuf);
    glDeleteBuffers(1,&histBuf);
    glDeleteBuffers(1,&ofsBuf);

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
    densityKernel->setRadius(support);
    gradKernel->setRadius(support);
    viscKernel->setRadius(support);
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


