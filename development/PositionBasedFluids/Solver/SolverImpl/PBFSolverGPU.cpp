#include"PBFSolverGPU.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include<list>
#include<iostream>
#include<GL/glew.h>

PBFSolverGPU::PBFSolverGPU(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver(densityKernel,gradKernel,viscKernel)
{
    this->spatialHashMap = new SpatialHashMap3D(2000,2*densityKernel->getRadius());
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

PBFSolverGPU::PBFSolverGPU(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations) : AbstractSolver(densityKernel,gradKernel,viscKernel)
{
    this->constraints = constraints;
    this->spatialHashMap = new SpatialHashMap3D(50000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
}

void PBFSolverGPU::init(std::vector<Particle>& particles)
{

    /*for(unsigned int p=0;p<particles.size();p++)
    {
        this->spatialHashMap->insert(particles[p]);
    }*/
    this->spatialHashMap->parallelInsert(particles);
}

void PBFSolverGPU::solve(std::vector<Particle>& particles)
{
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

    std::vector<std::list<Particle>> neighbors(particles.size());
    std::vector<glm::vec3> displacement(particles.size());

    //Apply external forces
    init(particles);
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        glm::vec3 accForces = glm::vec3(0.0,0.0,0.0);
        for(std::vector<AbstractForce*>::iterator f = externalForces.begin();f!=externalForces.end();f++)
        {
            accForces += (*f)->execute(particles[p].pos);
        }
        particles[p].vel += timestep*accForces;
        particles[p].tempPos        = particles[p].pos + timestep*particles[p].vel;
    }
    computeProgram->uploadUnsignedInt("taskId",0);

    //Neighbor search
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        std::list<unsigned int> candidates = spatialHashMap->find(Particle(particles[p].tempPos));
        for(std::list<unsigned int>::iterator it=candidates.begin();it!=candidates.end();it++)
        {
            if(particles[p].index!=*it)
            {
                Particle part = particles[*it];
                if(glm::length(particles[p].pos-part.pos)<=kernelSupport)
                {
                    neighbors[p].push_back(part);
                }
            }
        }
        neighbors[p];
    }

    //Solver iteration loop
    for(unsigned int i=0;i<iterations;i++)
    {
        //Calculate Correction
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            for(std::vector<AbstractConstraint*>::iterator cit=constraints.begin();cit!=constraints.end();cit++)
            {
                particles[p].density = (*cit)->execute(Particle(particles[p].tempPos),neighbors[p]);
                particles[p].lambda = -particles[p].density/((*cit)->gradientSum(Particle(particles[p].tempPos),neighbors[p])+cfmRegularization);
            }
        }

        //Update Particle Position
        float invRestDensity = (1.0f/restDensity);
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            displacement[p] = glm::vec3(0.0,0.0,0.0);
            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                float sCorr = -corrConst*std::pow((densityKernel->execute(particles[p].tempPos-n->pos)/densityKernel->execute(glm::vec3(kernelSupport,0.0,0.0)*corrDist)),corrExp);

                displacement[p] += (particles[p].lambda+n->lambda+sCorr)*gradKernel->grad(particles[p].tempPos-n->pos);
            }
            displacement[p] = invRestDensity*displacement[p];

            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                if(glm::length((particles[p].tempPos+displacement[p])-n->pos)-2*0.05<0.0)
                {
                    displacement[p] += glm::length((particles[p].tempPos+displacement[p])-n->pos)*glm::normalize((particles[p].tempPos+displacement[p])-n->pos);
                }
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0))+1.5f<0)
            {
                displacement[p]+=-2.0f*((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.5f)*glm::vec3(0.0,1.0,0.0);
            }
            /*if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,-1.0,0.0))+1.5f<0)
            {
                //std::cout<<((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.0f)<<std::endl;
                displacement[p]+=-2.0f*((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,-1.0,0.0)))+1.5f)*glm::vec3(0.0,-1.0,0.0);
                //displacement[p]-=particles[p].tempPos;
            }*/

            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.5f)*glm::vec3(-1.0,0.0,0.0);
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.5f)*glm::vec3(+1.0,0.0,0.0);
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,-1.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,-1.0))+1.5f)*glm::vec3(0.0,0.0,-1.0);
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.5f)*glm::vec3(0.0,0.0,+1.0);
            }
        }

        //Update Temporary Particle Positions
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            particles[p].tempPos += displacement[p];
        }
    }

    //Update particle velocities and positions
    float invTimestep = (1.0f/timestep);
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        particles[p].vel = invTimestep*(particles[p].tempPos-particles[p].pos);
    }
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        //XSPH Artificial Viscosity
        //Calculate curl
        particles[p].curl = glm::vec3(0.0,0.0,0.0);
        glm::vec3 velAccum(0.0f,0.0f,0.0f);
        for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
        {
            particles[p].curl += glm::cross(n->vel-particles[p].vel,viscKernel->grad(particles[p].tempPos-n->pos));
            velAccum += (n->vel-particles[p].vel)*viscKernel->execute(particles[p].tempPos-n->pos);
        }
        if(glm::length(particles[p].curl)>0.0001f)
        {
            particles[p].vel += timestep*artVort*glm::cross(glm::normalize(particles[p].curl),particles[p].curl);
        }
        particles[p].vel += timestep*artVisc*velAccum;
        particles[p].pos  = particles[p].tempPos;
    }
    spatialHashMap->clear();
}
