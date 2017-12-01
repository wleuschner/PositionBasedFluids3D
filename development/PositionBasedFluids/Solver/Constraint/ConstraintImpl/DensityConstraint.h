#ifndef __DENSITY_CONSTRAINT_H
#define __DENSITY_CONSTRAINT_H
#include"../AbstractConstraint.h"
#include"../../Kernel/AbstractKernel.h"


class DensityConstraint : AbstractConstraint
{
public:
    DensityConstraint(AbstractKernel* kernel);
    float execute(const Particle& x,std::list<Particle> particles);
    float gradientSum(const Particle& x,std::list<Particle> particles);
private:
    float restDensity;
    AbstractKernel* kernel;
};

#endif
