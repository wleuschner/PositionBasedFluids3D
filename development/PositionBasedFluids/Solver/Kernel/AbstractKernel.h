#ifndef __ABSTRACT_KERNEL_H
#define __ABSTRACT_KERNEL_H
#include<glm/glm.hpp>

class AbstractKernel
{
public:
    AbstractKernel(float h);
    virtual float execute(const glm::vec3& r) = 0;
    virtual glm::vec3 grad(const glm::vec3& r) = 0;

    void setRadius(float h);
    float getRadius();
protected:
    float h;
};

#endif
