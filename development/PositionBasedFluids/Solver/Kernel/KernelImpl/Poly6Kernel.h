#ifndef __POLY_6_KERNEL_H
#define __POLY_6_KERNEL_H
#include "../AbstractKernel.h"

class Poly6Kernel : AbstractKernel
{
public:
    Poly6Kernel(float h);
    float execute(glm::vec3 r);
    glm::vec3 gradient(glm::vec3 r);
private:
};

#endif
