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
    unsigned int index; //0
    unsigned int bucket; //4
    float lambda; //8
    float mass; //12
    float density; //16
    float pad[3]; //20

    glm::vec3 pos; //20
    float posPad;
    glm::vec3 vel; //36
    float velPad;


    glm::vec3 curl; //52
    float curPad;
    glm::vec3 tempPos; //66
    float tempPosPad;
    glm::vec3 displacement;
    float displacementPad;

    //float pad[3]; //
};

#endif
