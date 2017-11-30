#ifndef __PARTICLE_H
#define __PARTICLE_H
#include<glm/glm.hpp>

class Particle
{
public:
    glm::vec3 pos;
    glm::vec3 vel;
    float density;
};

#endif
