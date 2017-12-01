#ifndef __ABSTRACT_KERNEL_H
#define __ABSTRACT_KERNEL_H
#include<glm/glm.hpp>

class AbstractKernel
{
public:
    AbstractKernel(float h);
    virtual float execute(float r) = 0;
    virtual float derivation(float r) = 0;
    virtual glm::vec3 grad1(const glm::vec3& x1,const glm::vec3& x2) = 0;
    virtual glm::vec3 grad2(const glm::vec3& x1,const glm::vec3& x2) = 0;
protected:
    float h;
};

#endif
