#ifndef __ABSTRACT_SOLVER_H
#define __ABSTRACT_SOLVER_H
#include "Force/AbstractForce.h"
#include "Kernel/AbstractKernel.h"
#include "Particle.h"
#include <vector>

class AbstractSolver
{
public:
    AbstractSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel);

    virtual void init(std::vector<Particle>& particles) = 0;
    virtual void solve(std::vector<Particle>& particles) = 0;

    virtual void setDensityKernel(int id)=0;
    virtual void setGradKernel(int id)=0;
    virtual void setViscKernel(int id)=0;

    virtual void setNumIterations(unsigned int iterations)=0;
    virtual int getNumIterations()=0;

    virtual void setTimestep(float timestep)=0;
    virtual float getTimestep()=0;

    virtual void setKernelSupport(float support)=0;
    virtual float getKernelSupport()=0;

    virtual void setRestDensity(float density)=0;
    virtual float getRestDensity()=0;

    virtual void setArtVisc(float artVisc)=0;
    virtual float getArtVisc()=0;

    virtual void setArtVort(float artVort)=0;
    virtual float getArtVort()=0;

    virtual void setCfmRegularization(float cfm)=0;
    virtual float getCfmRegularization()=0;

    virtual void setCorrConst(float corrConst)=0;
    virtual float getCorrConst()=0;

    virtual void setCorrDist(float corrDist)=0;
    virtual float getCorrDist()=0;

    virtual void setCorrExp(float corrExp)=0;
    virtual float getCorrExp()=0;

protected:
    std::vector<AbstractForce*> externalForces;
    float timestep;
    unsigned int iterations;
    float kernelSupport;
    float restDensity;
    float artVisc;
    float artVort;
    float cfmRegularization;
    float corrConst;
    float corrDist;
    float corrExp;

    AbstractKernel* densityKernel;
    AbstractKernel* gradKernel;
    AbstractKernel* viscKernel;
private:
};

#endif
