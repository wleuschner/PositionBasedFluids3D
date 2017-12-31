#include"Poly6Kernel.h"
#include<cmath>
#include<iostream>

Poly6Kernel::Poly6Kernel(float h) : AbstractKernel(h)
{

}

float Poly6Kernel::execute(const glm::vec3& r)
{
    if(glm::length(r)>=h)
    {
        return 0;
    }
    float result = (315.0/(64*M_1_PI*std::pow(h,9)))*std::pow((h*h-glm::dot(r,r)),3);
    if(result!=result)
    {
        std::cout<<"KERNEL BROKEN"<<std::endl;
    }
    return result;
}

glm::vec3 Poly6Kernel::grad(const glm::vec3& r)
{
    if(glm::length(r)>=h)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
    return glm::vec3(-(945*(r.x)*std::pow((-std::pow((r.x),2)+std::pow((r.y),2)+std::pow((r.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(r.y)*std::pow((-std::pow((r.x),2)+std::pow((r.y),2)+std::pow((r.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(r.z)*std::pow((-std::pow((r.x),2)+std::pow((r.y),2)+std::pow((r.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)));


}
