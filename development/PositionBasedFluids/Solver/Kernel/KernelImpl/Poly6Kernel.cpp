#include"Poly6Kernel.h"
#include<cmath>

Poly6Kernel::Poly6Kernel(float h) : AbstractKernel(h)
{

}

float Poly6Kernel::execute(glm::vec3 r)
{
    return (315.0/(64*M_1_PI*std::pow(h,9)))*std::pow((h*h-glm::dot(r,r)),3);
}

glm::vec3 Poly6Kernel::gradient(glm::vec3 r)
{
    return glm::vec3(4*r.x*(-std::pow(h,2)+std::pow(r.x,2)+std::pow(r.y,2)+std::pow(r.z,2)),
                     4*r.y*(-std::pow(h,2)+std::pow(r.x,2)+std::pow(r.y,2)+std::pow(r.z,2)),
                     4*r.z*(-std::pow(h,2)+std::pow(r.x,2)+std::pow(r.y,2)+std::pow(r.z,2)));
}
