#include<GL/glew.h>
#include"PBFSolverGPU.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../../Graphics/AABB/AABB.h"
#include<list>
#include<iostream>
#include<GL/glew.h>

PBFSolverGPU::PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    this->spatialHashMap = new SpatialHashMap3D(particles,2000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
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
    this->spatialHashMap = new SpatialHashMap3D(particles,50000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
}

void PBFSolverGPU::init()
{

    /*for(unsigned int p=0;p<particles.size();p++)
    {
        this->spatialHashMap->insert(particles[p]);
    }*/
    this->spatialHashMap->update();
}

void PBFSolverGPU::solve()
{
    AABB aabb = AABB(glm::vec3(-1.0,-1.5,-1.0),glm::vec3(1.0,1.5,1.0));
    glm::ivec3 dimSize;
    const glm::vec3 ext = aabb.getExtent();
    const glm::vec3 min = aabb.min;
    dimSize.x = std::ceil(ext.x/kernelSupport);
    dimSize.y = std::ceil(ext.y/kernelSupport);
    dimSize.z = std::ceil(ext.z/kernelSupport);
    unsigned int elems = dimSize.x*dimSize.y*dimSize.z;
    std::cout<<dimSize.x<<" "<<dimSize.y<<" "<<dimSize.z<<std::endl;

    //Setup Compute Shader
    std::vector<unsigned int> bufData(elems);

    unsigned int histBuf;
    glGenBuffers(1,&histBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,histBuf);
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



    GLsync syncObj;

    computeProgram->uploadUnsignedInt("taskId",0);
    computeProgram->dispatch(particles.size(),1,1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    computeProgram->uploadUnsignedInt("taskId",1);
    computeProgram->dispatch(particles.size(),1,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,histBuf);
    unsigned int* ptr = (unsigned int *) glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );
    for(unsigned int i=0;i<elems;i++)
    {
        std::cout<<ptr[i]<<" ";
    }
    glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
    std::cout<<std::endl;

    //Iterate
    //glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    for(unsigned i=0;i<iterations;i++)
    {
        computeProgram->uploadUnsignedInt("taskId",3);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        computeProgram->uploadUnsignedInt("taskId",4);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        computeProgram->uploadUnsignedInt("taskId",2);
        computeProgram->dispatch(particles.size(),1,1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    }
    //glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
    glDeleteBuffers(1,&histBuf);

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
    this->spatialHashMap->setNewCellSize(support);
}

float PBFSolverGPU::getKernelSupport()
{
    return kernelSupport;
}

void PBFSolverGPU::setRestDensity(float density)
{
    ((DensityConstraint*)this->constraints[0])->setRestDensity(density);
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


