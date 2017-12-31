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
    glm::vec3 grad(0.0,0.0,0.0);
    for(std::list<Particle>::const_iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad += gradKernel->grad(x.pos-pit->pos);
    }
    grad = (1.0f/restDensity)*grad;
    result += glm::dot(grad,grad);
    if(result!=result)
    {
        std::cout<<"RESULT BROKEN"<<std::endl;
    }
    for(std::list<Particle>::const_iterator pit=particles.begin();pit!=particles.end();pit++)
    {
        grad = (1.0f/restDensity)*(-gradKernel->grad(x.pos-pit->pos));
        result += glm::dot(grad,grad);
    }
    if(result!=result)
    {
        std::cout<<"FINAL RESULT BROKEN"<<std::endl;
    }
    return result;
}
