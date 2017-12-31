#ifndef __SPIKY_KERNEL_H
#define __SPIKY_KERNEL_H
#include "../AbstractKernel.h"

class SpikyKernel : AbstractKernel
{
public:
    SpikyKernel(float h);
    float execute(const glm::vec3& r);
    glm::vec3 grad(const glm::vec3& r);
private:
};

#endif
