#ifndef __ABSTRACT_SOLVER_H
#define __ABSTRACT_SOLVER_H
#include "Force/AbstractForce.h"
#include "Particle.h"
#include <vector>

class AbstractSolver
{
public:
    AbstractSolver();
    virtual void solve(std::vector<Particle>& particles) = 0;
protected:
    std::vector<AbstractForce> externalForces;
    float timestep;
private:
};

#endif
