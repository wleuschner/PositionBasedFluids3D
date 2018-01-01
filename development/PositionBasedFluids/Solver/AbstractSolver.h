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

    void setNumIterations(unsigned int iterations);
    int getNumIterations();

    void setTimestep(float timestep);
    float getTimestep();

    void setKernelSupport(float support);
    float getKernelSupport();

    void setRestDensity(float density);
    float getRestDensity();

    void setArtVisc(float artVisc);
    float getArtVisc();

    void setArtVort(float artVort);
    float getArtVort();

    void setCfmRegularization(float cfm);
    float getCfmRegularization();

    void setCorrConst(float corrConst);
    float getCorrConst();

    void setCorrDist(float corrDist);
    float getCorrDist();

    void setCorrExp(float corrExp);
    float getCorrExp();

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
