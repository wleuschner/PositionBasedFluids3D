#include"DensityConstraint.h"

DensityConstraint::DensityConstraint(AbstractKernel* kernel)
{
    this->kernel = kernel;
    this->restDensity = 1000.0;
}

float DensityConstraint::execute(const Particle& x,std::list<Particle> particles)
{
    float density = 0.0;
    for(std::list<Particle>::iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        density += pit->mass * kernel->execute(x.pos-pit->pos);
    }
    return (density/restDensity)-1.0;
}

glm::vec3 DensityConstraint::gradient(const Particle& x,std::list<Particle> particles)
{
}
