#include"AABB.h"

AABB::AABB()
{
    max = glm::vec3(0.0,0.0,0.0);
    min = glm::vec3(0.0,0.0,0.0);
}

AABB::AABB(const glm::vec3& min,const glm::vec3& max)
{
    this->max = max;
    this->min = min;
}

const glm::vec3 AABB::getCenter() const
{
    return min+0.5f*(max-min);
}

const glm::vec3 AABB::getExtent() const
{
    return max-min;
}
