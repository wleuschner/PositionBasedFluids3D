#ifndef __ABSTRACT_CONSTRAINT_H
#define __ABSTRACT_CONSTRAINT_H
#include"../Particle.h"
#include<glm/glm.hpp>
#include<list>

class AbstractConstraint
{
public:
    AbstractConstraint();
    virtual float execute(const Particle& x,std::list<Particle> particles) = 0;
    virtual glm::vec3 gradient(const Particle& x,std::list<Particle> particles) = 0;
private:
};

#endif
