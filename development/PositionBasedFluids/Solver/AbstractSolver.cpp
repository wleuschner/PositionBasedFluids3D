#include"AbstractSolver.h"
#include"Force/ForceImpl/GravityForce.h"

AbstractSolver::AbstractSolver(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel) : particles(particles)
{
    this->restDensity = 7440.0;
    this->iterations = 4;
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->viscKernel = viscKernel;
    this->timestep = 0.00084;
    GravityForce* grav = new GravityForce();
    this->externalForces.push_back((AbstractForce*)grav);
    //this->cfmRegularization = 1500;
    this->cfmRegularization = 600;
    this->artVisc = 0.01;
    this->artVort = 0.01;
    this->corrConst = 0.01;
    this->corrExp = 4;
    this->corrDist = 0.01;
    this->particleSize = 0.0005;
    this->kernelSupport=densityKernel->getRadius();
}
