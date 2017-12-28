#include"GravityForce.h"

GravityForce::GravityForce() : AbstractForce()
{
}

glm::vec3 GravityForce::execute(const glm::vec3 &pos)
{
    return glm::vec3(0.0,-G,0.0);
}
