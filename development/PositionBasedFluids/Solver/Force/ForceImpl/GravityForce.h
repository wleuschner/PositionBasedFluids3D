#ifndef __GRAVITY_FORCE_H
#define __GRAVITY_FORCE_H
#include"../AbstractForce.h"
#include<glm/glm.hpp>

class GravityForce : AbstractForce
{
public:
    GravityForce();
    glm::vec3 execute(const glm::vec3& pos);
private:
    const float G=9.81;
};

#endif
