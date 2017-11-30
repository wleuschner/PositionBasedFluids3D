#include"PBFSolver.h"
#include<list>

PBFSolver::PBFSolver(float timestep) : AbstractSolver()
{
    this->iterations = 10;
    this->timestep = timestep;
}

void PBFSolver::solve(std::vector<Particle>& particles)
{
    std::vector<std::list<Particle>> neighbors(particles.size());
    std::vector<glm::vec3> tempPos(particles.size());

    //Apply external forces
    for(unsigned int p=0;p<particles.size();p++)
    {
        glm::vec3 accForces = glm::vec3(0.0,0.0,0.0);
        for(std::vector<AbstractForce>::iterator f = externalForces.begin();f!=externalForces.end();f++)
        {
            accForces += f->execute(particles[p].pos);
        }
        particles[p].vel += timestep*accForces;
        tempPos[p]       += timestep*particles[p].vel;
    }

    //Neighbor search
    for(unsigned int p=0;p<particles.size();p++)
    {
        neighbors[p];
    }

    //Solver iteration loop
    for(unsigned int i=0;i<iterations;i++)
    {
        //Calculate Correction
        for(unsigned int p=0;p<particles.size();p++)
        {
            for(std::vector<AbstractConstraint*>::iterator cit=constraints.begin();cit!=constraints.end();cit++)
            {
                float lambda = (*cit)->execute(particles[p],neighbors[p]);
            }
        }
    }

    //Update particle velocities and positions
    for(unsigned int p=0;p<particles.size();p++)
    {
        particles[p].vel = (1.0f/timestep)*(tempPos[p]-particles[p].pos);
        particles[p].pos = tempPos[p];
    }
}
