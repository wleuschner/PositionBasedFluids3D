#include"PBFSolver.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../Force/ForceImpl/GravityForce.h"
#include<list>
#include<iostream>

PBFSolver::PBFSolver(AbstractKernel* kernel,float timestep) : AbstractSolver()
{
    this->iterations = 10;
    this->kernel = kernel;
    this->timestep = timestep;
    this->spatialHashMap = new SpatialHashMap3D(100,kernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(kernel);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 0.01;
    this->artVisc = 0.01;
    this->corrConst = 0.1;
    this->corrExp = 4;
    this->corrDist = 0.1*kernel->getRadius();
    this->kernelSupport=kernel->getRadius();
}

PBFSolver::PBFSolver(AbstractKernel* kernel,float timestep,int iterations) : AbstractSolver()
{
    this->iterations = iterations;
    this->kernel = kernel;
    this->timestep = timestep;
    this->spatialHashMap = new SpatialHashMap3D(100,kernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(kernel);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 0.01;
    this->artVisc = 0.01;
    this->corrConst = 0.1;
    this->corrExp = 4;
    this->corrDist = 0.1*kernel->getRadius();
    this->kernelSupport=kernel->getRadius();
}

PBFSolver::PBFSolver(AbstractKernel* kernel,std::vector<AbstractConstraint*> constraints,float timestep) : AbstractSolver()
{
    this->iterations = 10;
    this->kernel = kernel;
    this->timestep = timestep;
    this->constraints = constraints;
    this->spatialHashMap = new SpatialHashMap3D(100,kernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(kernel);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 0.01;
    this->artVisc = 0.01;
    this->corrConst = 0.1;
    this->corrExp = 4;
    this->corrDist = 0.1*kernel->getRadius();
    this->kernelSupport=kernel->getRadius();
}

PBFSolver::PBFSolver(AbstractKernel* kernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations) : AbstractSolver()
{
    this->iterations = iterations;
    this->kernel = kernel;
    this->timestep = timestep;
    this->constraints = constraints;
    this->spatialHashMap = new SpatialHashMap3D(100,kernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(kernel);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 0.01;
    this->artVisc = 0.01;
    this->corrConst = 0.1;
    this->corrExp = 4;
    this->corrDist = 0.1*kernel->getRadius();
    this->kernelSupport=kernel->getRadius();
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
    for(unsigned int p=0;p<particles.size();p++)
    {
        this->spatialHashMap->insert(particles[p]);
    }
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
        std::list<Particle> candidates = spatialHashMap->find(Particle(tempPos[p]));
        for(std::list<Particle>::iterator it=candidates.begin();it!=candidates.end();it++)
        {
            if(particles[p].index!=it->index)
            {
                if(glm::length(particles[p].pos-it->pos)<kernelSupport)
                {
                    neighbors[p].push_back(*it);
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
                particles[p].lambda = -(*cit)->execute(particles[p],neighbors[p])/((*cit)->gradientSum(particles[p],neighbors[p])+cfmRegularization);
                if(particles[p].lambda!=particles[p].lambda)
                {
                    std::cout<<"NAN VALUE "<<p<<std::endl;
                }
            }
        }

        //Update Particle Position
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            displacement[p] = glm::vec3(0.0,0.0,0.0);
            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                float sCorr = -corrConst*std::pow((kernel->execute(glm::length(tempPos[p]-n->pos))/kernel->execute(corrDist)),corrExp);
                if(sCorr!=sCorr)
                {
                    std::cout<<"NAN VALUE CORRECTION"<<p<<" "<<corrDist<<" "<<corrExp<<std::endl;
                }
                displacement[p] += (particles[p].lambda+n->lambda+sCorr)*kernel->grad2(particles[p].pos,n->pos);
            }
            displacement[p] = (1.0f/10.0f)*displacement[p];
            if(displacement[p]!=displacement[p])
            {
                std::cout<<"NAN VALUE DISPLACEMENT"<<p<<std::endl;
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

        //XSPH Artificial Viscosity
        glm::vec3 velAccum(0.0f,0.0f,0.0f);
        for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
        {
            velAccum += (n->vel-particles[p].vel)*kernel->execute(glm::length(tempPos[p]-n->pos));
        }
        particles[p].vel += artVisc*velAccum;
        //particles[p].vel += velAccum;
        particles[p].pos  = tempPos[p];
    }
    spatialHashMap->clear();
}
