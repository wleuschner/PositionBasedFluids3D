#ifndef __LIGHT_H
#define __LIGHT_H
#include<glm/glm.hpp>

class Light
{
public:
    Light();
private:
    glm::vec3 pos;
    glm::vec3 amb;
    glm::vec3 diff;
    glm::vec3 spec;
    float shininess;
};

#endif
