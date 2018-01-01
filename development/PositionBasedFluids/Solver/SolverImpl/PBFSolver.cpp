#include"PBFSolver.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../Force/ForceImpl/GravityForce.h"
#include<list>
#include<iostream>

PBFSolver::PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver()
{
    //this->restDensity = 100.0f;
    this->restDensity = 700.0f;
    this->iterations = iterations;
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->viscKernel = viscKernel;
    this->timestep = timestep;
    this->spatialHashMap = new SpatialHashMap3D(2000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 600;
    this->artVisc = 0.01;
    this->artVort = 0.01;
    this->corrConst = 0.001;
    this->corrExp = 4;
    this->corrDist = 0.01*densityKernel->getRadius();
    this->kernelSupport=densityKernel->getRadius();
}

PBFSolver::PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations) : AbstractSolver()
{
    this->restDensity = 10.0f;
    this->iterations = iterations;
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->viscKernel = viscKernel;
    this->timestep = timestep;
    this->constraints = constraints;
    this->spatialHashMap = new SpatialHashMap3D(50000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 1.0;
    this->artVisc = 0.01;
    this->corrConst = 0.1;
    this->corrExp = 4;
    this->corrDist = 0.1*densityKernel->getRadius();
    this->kernelSupport=densityKernel->getRadius();
}

void PBFSolver::setNumIterations(unsigned int iterations)
{
    this->iterations = iterations;
}

int PBFSolver::getNumIterations()
{
    return iterations;
}

void PBFSolver::init(std::vector<Particle>& particles)
{

    /*for(unsigned int p=0;p<particles.size();p++)
    {
        this->spatialHashMap->insert(particles[p]);
    }*/
    this->spatialHashMap->parallelInsert(particles);
}

void PBFSolver::solve(std::vector<Particle>& particles)
{
    std::vector<std::list<Particle>> neighbors(particles.size());
    std::vector<glm::vec3> tempPos(particles.size());
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
        tempPos[p]        = particles[p].pos + timestep*particles[p].vel;
    }

    //Neighbor search
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        std::list<unsigned int> candidates = spatialHashMap->find(Particle(tempPos[p]));
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
                particles[p].density = (*cit)->execute(Particle(tempPos[p]),neighbors[p]);
                particles[p].lambda = -particles[p].density/((*cit)->gradientSum(Particle(tempPos[p]),neighbors[p])+cfmRegularization);
            }
        }

        //Update Particle Position
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            displacement[p] = glm::vec3(0.0,0.0,0.0);
            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                float sCorr = -corrConst*std::pow((densityKernel->execute(tempPos[p]-n->pos)/densityKernel->execute(glm::vec3(kernelSupport,0.0,0.0)*corrDist)),corrExp);

                displacement[p] += (particles[p].lambda+n->lambda+sCorr)*gradKernel->grad(tempPos[p]-n->pos);
            }
            displacement[p] = (1.0f/restDensity)*displacement[p];

            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                if(glm::length((tempPos[p]+displacement[p])-n->pos)-2*0.05<0.0)
                {
                    displacement[p] += glm::length((tempPos[p]+displacement[p])-n->pos)*glm::normalize((tempPos[p]+displacement[p])-n->pos);
                }
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0))+1.5f<0)
            {
                displacement[p]+=-2.0f*((glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.5f)*glm::vec3(0.0,1.0,0.0);
            }
            /*if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,-1.0,0.0))+1.5f<0)
            {
                //std::cout<<((glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.0f)<<std::endl;
                displacement[p]+=-2.0f*((glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,-1.0,0.0)))+1.5f)*glm::vec3(0.0,-1.0,0.0);
                //displacement[p]-=tempPos[p];
            }*/

            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(-1.0,0.0,0.0))+1.5f)*glm::vec3(-1.0,0.0,0.0);
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(+1.0,0.0,0.0))+1.5f)*glm::vec3(+1.0,0.0,0.0);
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,-1.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,-1.0))+1.5f)*glm::vec3(0.0,0.0,-1.0);
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.5f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,+1.0))+1.5f)*glm::vec3(0.0,0.0,+1.0);
            }
        }

        //Update Temporary Particle Positions
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            tempPos[p] += displacement[p];
        }
    }

    //Update particle velocities and positions
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        particles[p].vel = (1.0f/timestep)*(tempPos[p]-particles[p].pos);
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
            particles[p].curl += glm::cross(n->vel-particles[p].vel,viscKernel->grad(tempPos[p]-n->pos));
            velAccum += (n->vel-particles[p].vel)*viscKernel->execute(tempPos[p]-n->pos);
        }
        if(glm::length(particles[p].curl)>0.0001)
        {
            particles[p].vel += timestep*artVort*glm::cross(glm::normalize(particles[p].curl),particles[p].curl);
        }
        particles[p].vel += timestep*artVisc*velAccum;
        particles[p].pos  = tempPos[p];
    }
    spatialHashMap->clear();
}
