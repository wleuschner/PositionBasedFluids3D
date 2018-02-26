#include"Particle.h"

Particle::Particle()
{
}

Particle::Particle(int index,glm::vec3 pos,glm::vec3 vel,float mass,float density,bool solid)
{
    this->index = index;
    this->pos = glm::vec3(pos);
    this->vel = glm::vec3(vel);
    this->mass = mass;
    this->density = density;
    if(solid)
    {
        this->fluid=0.0;//Fluid
        this->solid=1.0;//Solid
        this->gas=0.0;//Gas
    }
    else
    {
        this->fluid=1.0;//Fluid
        this->solid=0.0;//Solid;
        this->gas=0.0;//Gas
    }
}

Particle::Particle(glm::vec3 pos,glm::vec3 vel,float mass,float density)
{
    this->pos = glm::vec3(pos);
    this->vel = glm::vec3(vel);
    this->mass = mass;
    this->density = density;
}

Particle::Particle(glm::vec3 pos)
{
    this->pos = pos;
}
