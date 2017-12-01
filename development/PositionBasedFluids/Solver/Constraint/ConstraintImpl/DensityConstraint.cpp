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
        density += pit->mass * kernel->execute(glm::length(x.pos-pit->pos));
    }
    return (density/restDensity)-1.0;
}

float DensityConstraint::gradientSum(const Particle& x,std::list<Particle> particles)
{
    float result = 0.0;
    glm::vec3 grad(0.0,0.0,0.0);
    for(std::list<Particle>::iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad += kernel->grad1(x.pos,pit->pos);
    }
    grad = (1.0f/restDensity)*grad;
    result = glm::dot(grad,grad);
    for(std::list<Particle>::iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad = (1.0f/restDensity)*(-kernel->grad2(x.pos,pit->pos));
        result += glm::dot(grad,grad);
    }
    return result;
}
