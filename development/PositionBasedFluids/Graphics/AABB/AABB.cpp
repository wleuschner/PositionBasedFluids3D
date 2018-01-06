#include"AABB.h"

AABB::AABB()
{

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
