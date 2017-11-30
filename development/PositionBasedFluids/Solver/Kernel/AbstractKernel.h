#ifndef __ABSTRACT_KERNEL_H
#define __ABSTRACT_KERNEL_H
#include<glm/glm.hpp>

class AbstractKernel
{
public:
    AbstractKernel(float h);
    virtual float execute(float r) = 0;
protected:
    float h;
};

#endif
