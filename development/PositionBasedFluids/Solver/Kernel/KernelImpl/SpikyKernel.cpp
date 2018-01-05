#include"SpikyKernel.h"
#include<cmath>
#include<iostream>

SpikyKernel::SpikyKernel(float h) : AbstractKernel(h)
{

}

float SpikyKernel::execute(const glm::vec3& r)
{
    if(glm::length(r)>h)
    {
        return 0;
    }
    float result = (15.0/(M_PI*std::pow(h,6)))*std::pow((h-glm::length(r)),3);
    if(result!=result)
    {
        std::cout<<"GRAD KERNEL BROKEN"<<std::endl;
    }
    return result;
}

glm::vec3 SpikyKernel::grad(const glm::vec3& r)
{
    if(glm::length(r)>h || glm::length(r)==0)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
    return ((float)(-45.0f/(M_PI*(std::pow(h,6))))*((float)std::pow((h-glm::length(r)),2))*glm::normalize(r));
}

