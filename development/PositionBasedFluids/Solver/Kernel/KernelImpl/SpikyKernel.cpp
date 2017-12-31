#include"SpikyKernel.h"
#include<cmath>

SpikyKernel::SpikyKernel(float h) : AbstractKernel(h)
{

}

float SpikyKernel::execute(const glm::vec3& r)
{
    if(glm::length(r)>h)
    {
        return 0;
    }
    return (15.0/(M_1_PI*std::pow(h,6)))*std::pow((h-glm::length(r)),3);
}

glm::vec3 SpikyKernel::grad(const glm::vec3& r)
{
    if(glm::length(r)>h || glm::length(r)<0.0001)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
    return ((float)(-45.0f/(M_1_PI*(std::pow(h,6))))*((float)std::pow((h-glm::length(r)),2))*glm::normalize(r));
}

