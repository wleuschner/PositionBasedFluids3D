#include"SpikyKernel.h"
#include<cmath>

SpikyKernel::SpikyKernel(float h) : AbstractKernel(h)
{

}

float SpikyKernel::execute(float r)
{
    return (15.0/(M_1_PI*std::pow(h,6)))*std::pow((h-r),3);
}

float SpikyKernel::derivation(float r)
{
    return -45*std::pow((h-r),2)/(M_1_PI*std::pow(h,6));
}

glm::vec3 SpikyKernel::grad1(const glm::vec3& x1,const glm::vec3& x2)
{
    return glm::vec3(-(90*(x1.x-x2.x)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)),
                     -(90*(x1.y-x2.y)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)),
                     -(90*(x1.z-x2.z)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)));
}

glm::vec3 SpikyKernel::grad2(const glm::vec3& x1,const glm::vec3& x2)
{
    return glm::vec3(-(90*(x2.x-x1.x)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)),
                     -(90*(x2.y-x1.y)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)),
                     -(90*(x2.z-x1.z)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,6)));
}
