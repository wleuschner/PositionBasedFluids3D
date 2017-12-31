#ifndef __DENSITY_CONSTRAINT_H
#define __DENSITY_CONSTRAINT_H
#include"../AbstractConstraint.h"
#include"../../Kernel/AbstractKernel.h"


class DensityConstraint : AbstractConstraint
{
public:
    DensityConstraint(AbstractKernel* densityKernel,AbstractKernel* gradKernel,float restDensity);
    float execute(const Particle& x,const std::list<Particle>& particles);
    float gradientSum(const Particle& x,const std::list<Particle>& particles);
private:
    float restDensity;
    AbstractKernel* densityKernel;
    AbstractKernel* gradKernel;
};

#endif
