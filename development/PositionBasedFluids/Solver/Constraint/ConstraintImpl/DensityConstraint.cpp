#include"DensityConstraint.h"
#include<iostream>

DensityConstraint::DensityConstraint(AbstractKernel* densityKernel,AbstractKernel* gradKernel,float restDensity)
{
    this->densityKernel = densityKernel;
    this->gradKernel = gradKernel;
    this->restDensity = restDensity;
}

float DensityConstraint::execute(const Particle& x,const std::list<Particle>& particles)
{
    float density = 0.0;
    for(std::list<Particle>::const_iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        density += pit->mass * densityKernel->execute(x.pos-pit->pos);
    }
    return (density/restDensity)-1.0;
}

float DensityConstraint::gradientSum(const Particle& x,const std::list<Particle>& particles)
{
    float result = 0.0;
    float invRestDensity=(1.0f/restDensity);
    glm::vec3 grad(0.0,0.0,0.0);
    for(std::list<Particle>::const_iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad += gradKernel->grad(x.pos-pit->pos);
    }
    grad = invRestDensity*grad;
    result += glm::dot(grad,grad);

    for(std::list<Particle>::const_iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad = invRestDensity*(-gradKernel->grad(x.pos-pit->pos));
        result += glm::dot(grad,grad);
    }
    return result;
}

void DensityConstraint::setRestDensity(float density)
{
    this->restDensity=density;
}
