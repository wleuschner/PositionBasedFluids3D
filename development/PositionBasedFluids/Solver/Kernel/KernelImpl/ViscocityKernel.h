#ifndef __VISCOCITY_KERNEL_H
#define __VISCOCITY_KERNEL_H
#include "../AbstractKernel.h"

class ViscocityKernel : AbstractKernel
{
public:
    ViscocityKernel(float h);
    float execute(const glm::vec3& r);
    glm::vec3 grad(const glm::vec3& r);
    float laplacian(const glm::vec3 &r);
private:
};

#endif
