#include"Particle.h"

Particle::Particle()
{
}

Particle::Particle(int index,glm::vec3 pos,glm::vec3 vel,float mass,float density)
{
    this->index = index;
    this->pos = pos;
    this->vel = vel;
    this->mass = mass;
    this->density = density;
}

Particle::Particle(glm::vec3 pos,glm::vec3 vel,float mass,float density)
{
    this->pos = pos;
    this->vel = vel;
    this->mass = mass;
    this->density = density;
}

Particle::Particle(glm::vec3 pos)
{
    this->pos = pos;
}
