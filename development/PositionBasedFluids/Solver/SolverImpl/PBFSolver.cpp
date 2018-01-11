#include"PBFSolver.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../../SpatialStruct/RadixSort/RadixSort.h"
#include"../../SpatialStruct/SpatialHashMap/SpatialHashMap.h"
#include<glm/gtx/intersect.hpp>
#include<list>
#include<iostream>

PBFSolver::PBFSolver(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    //this->spatialHashMap = (AbstractSpatialStruct*)new SpatialHashMap3D(particles,2000,densityKernel->getRadius());
    this->spatialHashMap = (AbstractSpatialStruct*)new RadixSort(particles,densityKernel->getRadius(),AABB(glm::vec3(-2.0,-2.0,-2.0),glm::vec3(2.0,2.0,2.0)));
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
}

PBFSolver::PBFSolver(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations) : AbstractSolver(particles,densityKernel,gradKernel,viscKernel)
{
    this->constraints = constraints;
    this->spatialHashMap = (AbstractSpatialStruct*)new SpatialHashMap3D(particles,50000,densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
}

void PBFSolver::init()
{

    /*for(unsigned int p=0;p<particles.size();p++)
    {
        this->spatialHashMap->insert(particles[p]);
    }*/
    this->spatialHashMap->update();
}

void PBFSolver::solve()
{
    std::vector<std::list<Particle>> neighbors(particles.size());
    std::vector<glm::vec3> displacement(particles.size());

    //Apply external forces
    init();
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        glm::vec3 accForces = glm::vec3(0.0,0.0,0.0);
        for(std::vector<AbstractForce*>::iterator f = externalForces.begin();f!=externalForces.end();f++)
        {
            accForces += (*f)->execute(particles[p].pos);
        }
        particles[p].vel += accForces;
        particles[p].tempPos        = particles[p].pos + timestep*particles[p].vel;
    }

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
                if(glm::dot(particles[p].tempPos-part.pos,particles[p].tempPos-part.pos)<=kernelSupport*kernelSupport)
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
        /*TODO:
         * Fix Collision Response
         */
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

            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0))+1.5f<0)
            {
                //std::cout<<glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0))+0.5f<<std::endl;
                //displacement[p] = -1.0f*((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0)))+0.5f)*glm::vec3(0.0,1.0,0.0);

                glm::vec3 n1 = glm::vec3(0.0,1.0,0.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }

            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,-1.0,0.0))+1.5f<0)
            {
                //std::cout<<((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.0f)<<std::endl;
                //displacement[p] =-1.0f*((glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,-1.0,0.0)))+1.5f)*glm::vec3(0.0,-1.0,0.0);
                glm::vec3 n1 = glm::vec3(0.0,-1.0,0.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }

            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.5f<0)
            {
                //displacement[p]=-1.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.0f)*glm::vec3(-1.0,0.0,0.0);
                glm::vec3 n1 = glm::vec3(-1.0,0.0,0.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.5f<0)
            {
                //displacement[p]=-1.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.0f)*glm::vec3(+1.0,0.0,0.0);
                glm::vec3 n1 = glm::vec3(1.0,0.0,0.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,-1.5))+1.5f<0)
            {
                //displacement[p]=-1.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,-1.0))+1.0f)*glm::vec3(0.0,0.0,-1.0);
                glm::vec3 n1 = glm::vec3(0.0,0.0,-1.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }
            if(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.5f<0)
            {
                //displacement[p]=-1.0f*(glm::dot((particles[p].tempPos+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.0f)*glm::vec3(0.0,0.0,+1.0);
                glm::vec3 n1 = glm::vec3(0.0,0.0,1.0);
                glm::vec3 r = glm::normalize((particles[p].tempPos+displacement[p])-particles[p].pos);
                float t = -(1.5+glm::dot(n1,-1.5f*n1))/glm::dot(n1,r);
                displacement[p] = particles[p].pos+(r*t)-particles[p].tempPos;
            }

            float minDist = std::numeric_limits<float>::infinity();
            float rSumSquared = 4*particleSize*particleSize;
            glm::vec3 v = (particles[p].tempPos+displacement[p])-particles[p].pos;
            glm::vec3 n1 = glm::normalize(v);
            glm::vec3 tempDispl = displacement[p];
            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                glm::vec3 c = n->pos-particles[p].pos;
                float dist = glm::dot(n1,c);
                if(dist>0.0)
                {
                    float f = glm::dot(c,c)-(dist*dist);
                    if(f<rSumSquared)
                    {
                        float t = std::sqrt(rSumSquared-f);
                        float corr = (dist-t);
                        if(corr<minDist)
                        {
                            //std::cout<<corr<<std::endl;
                            tempDispl = particles[p].pos+(corr*n1)-particles[p].tempPos;

                            minDist=corr;
                        }
                    }
                }
            }
            displacement[p] = tempDispl;
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

void PBFSolver::setDensityKernel(int id)
{

}

void PBFSolver::setGradKernel(int id)
{

}

void PBFSolver::setViscKernel(int id)
{

}

void PBFSolver::setNumIterations(unsigned int iterations)
{
    this->iterations = iterations;
}

int PBFSolver::getNumIterations()
{
    return iterations;
}

void PBFSolver::setTimestep(float timestep)
{
    this->timestep = timestep;
}

float PBFSolver::getTimestep()
{
    return timestep;
}

void PBFSolver::setKernelSupport(float support)
{
    densityKernel->setRadius(support);
    gradKernel->setRadius(support);
    viscKernel->setRadius(support);
    this->kernelSupport = support;
    this->spatialHashMap->setNewCellSize(support);
}

float PBFSolver::getKernelSupport()
{
    return kernelSupport;
}

void PBFSolver::setRestDensity(float density)
{
    ((DensityConstraint*)this->constraints[0])->setRestDensity(density);
    this->restDensity = density;
}

float PBFSolver::getRestDensity()
{
    return restDensity;
}

void PBFSolver::setArtVisc(float artVisc)
{
    this->artVisc = artVisc;
}

float PBFSolver::getArtVisc()
{
    return artVisc;
}

void PBFSolver::setArtVort(float artVort)
{
    this->artVort = artVort;
}

float PBFSolver::getArtVort()
{
    return artVort;
}

void PBFSolver::setCfmRegularization(float cfm)
{
    this->cfmRegularization = cfm;
}

float PBFSolver::getCfmRegularization()
{
    return cfmRegularization;
}

void PBFSolver::setCorrConst(float corrConst)
{
    this->corrConst = corrConst;
}

float PBFSolver::getCorrConst()
{
    return corrConst;
}

void PBFSolver::setCorrDist(float corrDist)
{
    this->corrDist = corrDist;
}

float PBFSolver::getCorrDist()
{
    return corrDist;
}

void PBFSolver::setCorrExp(float corrExp)
{
    this->corrExp = corrExp;
}

float PBFSolver::getCorrExp()
{
    return corrExp;
}

void PBFSolver::setPartSize(float size)
{
    this->particleSize=size;
}

float PBFSolver::getPartSize()
{
    return particleSize;
}
