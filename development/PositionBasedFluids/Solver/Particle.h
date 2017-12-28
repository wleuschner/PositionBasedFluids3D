#ifndef __PARTICLE_H
#define __PARTICLE_H
#include<glm/glm.hpp>

class Particle
{
public:
    Particle();
    Particle(int index,glm::vec3 pos,glm::vec3 vel,float mass,float density);
    Particle(glm::vec3 pos,glm::vec3 vel,float mass,float density);
    Particle(glm::vec3 pos);
    unsigned int index;
    glm::vec3 pos;
    glm::vec3 vel;
    float lambda;
    float mass;
    float density;
};

#endif
