#ifndef __POLY_6_KERNEL_H
#define __POLY_6_KERNEL_H
#include "../AbstractKernel.h"

class SpikyKernel : AbstractKernel
{
public:
    SpikyKernel(float h);
    float execute(float r);
    float derivation(float r);
    glm::vec3 grad1(const glm::vec3& x1,const glm::vec3& x2);
    glm::vec3 grad2(const glm::vec3& x1,const glm::vec3& x2);
private:
};

#endif
