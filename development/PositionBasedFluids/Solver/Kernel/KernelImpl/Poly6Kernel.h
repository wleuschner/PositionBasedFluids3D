#ifndef __POLY_6_KERNEL_H
#define __POLY_6_KERNEL_H
#include "../AbstractKernel.h"

class Poly6Kernel : AbstractKernel
{
public:
    Poly6Kernel(float h);
    float execute(const glm::vec3& r);
    glm::vec3 grad(const glm::vec3& r);
private:
};

#endif
