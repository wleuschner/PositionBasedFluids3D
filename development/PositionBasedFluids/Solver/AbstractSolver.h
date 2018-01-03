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

    virtual void setNumIterations(unsigned int iterations);
    virtual int getNumIterations();

    virtual void setTimestep(float timestep);
    virtual float getTimestep();

    virtual void setKernelSupport(float support);
    virtual float getKernelSupport();

    virtual void setRestDensity(float density);
    virtual float getRestDensity();

    virtual void setArtVisc(float artVisc);
    virtual float getArtVisc();

    virtual void setArtVort(float artVort);
    virtual float getArtVort();

    virtual void setCfmRegularization(float cfm);
    virtual float getCfmRegularization();

    virtual void setCorrConst(float corrConst);
    virtual float getCorrConst();

    virtual void setCorrDist(float corrDist);
    virtual float getCorrDist();

    virtual void setCorrExp(float corrExp);
    virtual float getCorrExp();

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
