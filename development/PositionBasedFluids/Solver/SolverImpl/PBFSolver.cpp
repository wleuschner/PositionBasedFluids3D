#include"PBFSolver.h"
#include"../Constraint/ConstraintImpl/DensityConstraint.h"
#include"../Force/ForceImpl/GravityForce.h"
#include<list>
#include<iostream>

PBFSolver::PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations) : AbstractSolver()
{
    //this->restDensity = 100.0f;
    this->restDensity = 3500.0f;
    this->iterations = iterations;
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->viscKernel = viscKernel;
    this->timestep = timestep;
    this->spatialHashMap = new SpatialHashMap3D(50000,2*densityKernel->getRadius());
    DensityConstraint* ds = new DensityConstraint(densityKernel,gradKernel,this->restDensity);
    this->constraints.push_back((AbstractConstraint*)ds);
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    this->cfmRegularization = 600;
    this->artVisc = 0.01;
    this->corrConst = 0.0001;
    this->corrExp = 4;
    this->corrDist = 0.3*densityKernel->getRadius();
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
                particles[p].lambda = -(*cit)->execute(Particle(tempPos[p]),neighbors[p])/((*cit)->gradientSum(Particle(tempPos[p]),neighbors[p])+cfmRegularization);
                particles[p].density = (*cit)->execute(Particle(tempPos[p]),neighbors[p]);
                //std::cout<<(*cit)->execute(particles[p],neighbors[p])<<" "<<((*cit)->gradientSum(particles[p],neighbors[p]))<<" "<<particles[p].lambda<<std::endl;
                /*
                if(particles[p].lambda!=particles[p].lambda)
                {
                    std::cout<<"NAN VALUE LAMBDA "<<particles[p].lambda<<" "<<std::endl;
                }
                */
            }
        }

        //Update Particle Position
        #pragma omp parallel for
        for(unsigned int p=0;p<particles.size();p++)
        {
            displacement[p] = glm::vec3(0.0,0.0,0.0);
            float test = 0.0;
            for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
            {
                float sCorr = -corrConst*std::pow((densityKernel->execute(tempPos[p]-n->pos)/densityKernel->execute(glm::vec3(kernelSupport,0.0,0.0)*corrDist)),corrExp);
                //float sCorr = 0;

                displacement[p] += (particles[p].lambda+n->lambda+sCorr)*gradKernel->grad(tempPos[p]-n->pos);
                //test+=particles[p].lambda+n->lambda;
                //std::cout<<"LAMBDA "<<particles[p].lambda<<" "<<n->lambda<<" "<<particles[p].lambda+n->lambda<<" "<<test<<std::endl;
                /*
                if(displacement[p]!=displacement[p])
                {
                    std::cout<<"NAN VALUE DISPLACEMENT "<<particles[p].lambda<<" "<<n->lambda<<std::endl;
                }
                if(sCorr!=sCorr)
                {
                    std::cout<<"NAN VALUE CORRECTION "<<particles[p].lambda<<" "<<n->lambda<<std::endl;
                }*/
            }
            displacement[p] = (1.0f/restDensity)*displacement[p];
/*
            if(displacement[p]!=displacement[p])
            {
                std::cout<<"NAN VALUE DISPLACEMENT"<<p<<std::endl;
            }*/
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0))+1.0f<0)
            {
                //std::cout<<((glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.0f)<<std::endl;
                displacement[p]+=-2.0f*((glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,1.0,0.0)))+1.0f)*glm::vec3(0.0,1.0,0.0);
                //displacement[p]-=tempPos[p];
            }

            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(-1.0,0.0,0.0))+50.0f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(-1.0,0.0,0.0))+50.0f)*glm::vec3(-1.0,0.0,0.0);
                /*displacement[p]+=(glm::dot((tempPos[p]+displacement[p]),glm::vec3(-1.0,0.0,0.0))+10.0f)*glm::vec3(-1.0,0.0,0.0);
                displacement[p]-=tempPos[p];*/
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(+1.0,0.0,0.0))+50.0f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(+1.0,0.0,0.0))+50.0f)*glm::vec3(+1.0,0.0,0.0);
                /*displacement[p]+=(glm::dot((tempPos[p]+displacement[p]),glm::vec3(+1.0,0.0,0.0))+10.0f)*glm::vec3(+1.0,0.0,0.0);
                displacement[p]-=tempPos[p];*/
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,-1.0))+10.0f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,-1.0))+10.0f)*glm::vec3(0.0,0.0,-1.0);
                /*displacement[p]+=(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,-1.0))+10.0f)*glm::vec3(0.0,0.0,-1.0);
                displacement[p]-=tempPos[p];*/
            }
            if(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,+1.0))+10.0f<0)
            {
                displacement[p]=-2.0f*(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,+1.0))+10.0f)*glm::vec3(0.0,0.0,+1.0);
                /*displacement[p]+=(glm::dot((tempPos[p]+displacement[p]),glm::vec3(0.0,0.0,+1.0))+10.0f)*glm::vec3(0.0,0.0,+1.0);
                displacement[p]-=tempPos[p];*/
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
        //Calculate curl
        particles[p].curl = glm::vec3(0.0,0.0,0.0);
        for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
        {
            particles[p].curl += glm::cross(n->vel-particles[p].vel,viscKernel->grad(tempPos[p]-n->pos));
        }
        //glm::vec3 vGrad = glm::normalize(particles[p].curl);
        //glm::vec3 fVort = 0.1f*(glm::cross(vGrad,particles[p].curl));
        //particles[p].vel += timestep*fVort;
    }
    #pragma omp parallel for
    for(unsigned int p=0;p<particles.size();p++)
    {
        glm::vec3 oGrad(0.0,0.0,0.0);
        for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
        {
            float lCurl = glm::length(particles[p].curl-n->curl);
            glm::vec3 r = -tempPos[p]+n->pos;
            oGrad += glm::vec3(lCurl/r.x,lCurl/r.y,lCurl/r.z);
        }
        oGrad = glm::normalize(oGrad);
        //particles[p].vel += timestep*0.01f*glm::cross(oGrad,particles[p].curl);
        //XSPH Artificial Viscosity
        glm::vec3 velAccum(0.0f,0.0f,0.0f);
        for(std::list<Particle>::iterator n=neighbors[p].begin();n!=neighbors[p].end();n++)
        {
            velAccum += (n->vel-particles[p].vel)*viscKernel->execute(tempPos[p]-n->pos);
        }
        particles[p].vel += timestep*artVisc*velAccum;
        //particles[p].vel += velAccum;
        particles[p].pos  = tempPos[p];
    }
    spatialHashMap->clear();
}
