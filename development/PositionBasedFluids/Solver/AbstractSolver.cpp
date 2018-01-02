#include"AbstractSolver.h"
#include"Force/ForceImpl/GravityForce.h"

AbstractSolver::AbstractSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel)
{
    this->restDensity = 1000.0f;
    this->iterations = 4;
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->viscKernel = viscKernel;
    this->timestep = 0.08;
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

void AbstractSolver::setNumIterations(unsigned int iterations)
{
    this->iterations = iterations;
}

int AbstractSolver::getNumIterations()
{
    return iterations;
}

void AbstractSolver::setTimestep(float timestep)
{
    this->timestep = timestep;
}

float AbstractSolver::getTimestep()
{
    return timestep;
}

void AbstractSolver::setKernelSupport(float support)
{
    densityKernel->setRadius(support);
    gradKernel->setRadius(support);
    viscKernel->setRadius(support);
    this->kernelSupport = support;
}

float AbstractSolver::getKernelSupport()
{
    return kernelSupport;
}

void AbstractSolver::setRestDensity(float density)
{
    this->restDensity = density;
}

float AbstractSolver::getRestDensity()
{
    return restDensity;
}

void AbstractSolver::setArtVisc(float artVisc)
{
    this->artVisc = artVisc;
}

float AbstractSolver::getArtVisc()
{
    return artVisc;
}

void AbstractSolver::setArtVort(float artVort)
{
    this->artVort = artVort;
}

float AbstractSolver::getArtVort()
{
    return artVort;
}

void AbstractSolver::setCfmRegularization(float cfm)
{
    this->cfmRegularization = cfm;
}

float AbstractSolver::getCfmRegularization()
{
    return cfmRegularization;
}

void AbstractSolver::setCorrConst(float corrConst)
{
    this->corrConst = corrConst;
}

float AbstractSolver::getCorrConst()
{
    return corrConst;
}

void AbstractSolver::setCorrDist(float corrDist)
{
    this->corrDist = corrDist;
}

float AbstractSolver::getCorrDist()
{
    return corrDist;
}

void AbstractSolver::setCorrExp(float corrExp)
{
    this->corrExp = corrExp;
}

float AbstractSolver::getCorrExp()
{
    return corrExp;
}
