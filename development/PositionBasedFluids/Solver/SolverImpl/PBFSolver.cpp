#include"PBFSolver.h"

PBFSolver::PBFSolver(float timestep) : AbstractSolver()
{
    this->iterations = 10;
    this->timestep = timestep;
}

void PBFSolver::solve(std::vector<Particle>& particles)
{
    //Apply external forces
    for(std::vector<Particle>::iterator p=particles.begin();p!=particles.end();p++)
    {
        glm::vec3 accForces = glm::vec3(0.0,0.0,0.0);
        for(std::vector<AbstractForce>::iterator f = externalForces.begin();f!=externalForces.end();f++)
        {
            accForces += f->execute(p->pos);
        }
        p->vel += timestep*accForces;
        p->pos += timestep*p->vel;
    }

    //Solver iteration loop
    for(unsigned int i=0;i<iterations;i++)
    {
    }

    //Update particle velocities and positions
    for(std::vector<Particle>::iterator p=particles.begin();p!=particles.end();p++)
    {
    }
}
