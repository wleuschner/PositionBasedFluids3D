#include"Poly6Kernel.h"
#include<cmath>

Poly6Kernel::Poly6Kernel(float h) : AbstractKernel(h)
{

}

float Poly6Kernel::execute(float r)
{
    if(r>h)
    {
        return 0;
    }
    return (315.0/(64*M_1_PI*std::pow(h,9)))*std::pow((h*h-r*r),3);
}

float Poly6Kernel::derivation(float r)
{
    if(r>h)
    {
        return 0;
    }
    return -((945*r*std::pow((h*h-r*r),2))/(32*M_1_PI*std::pow(h,9)));
}

glm::vec3 Poly6Kernel::grad1(const glm::vec3& x1,const glm::vec3& x2)
{
    if(glm::length(x1-x2)>h)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
    /*return glm::vec3(-(945*(x1.x-x2.x)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(x1.y-x2.y)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(x1.z-x2.z)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)));
*/
    /*return glm::vec3(-(1890*(x1.x-x2.x)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)),
                     -(1890*(x1.y-x2.y)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)),
                     -(1890*(x1.z-x2.z)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)));*/


    return -((float)(-45.0f/(M_1_PI*(std::pow(h,6))))*((float)std::pow((h-glm::length(x1-x2)),2))*glm::normalize(x2-x1));
}

glm::vec3 Poly6Kernel::grad2(const glm::vec3& x1,const glm::vec3& x2)
{
    if(glm::length(x1-x2)>h)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
/*    return glm::vec3(-(945*(x2.x-x1.x)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(x2.y-x1.y)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)),
                     -(945*(x2.z-x1.z)*std::pow((-std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)+h*h),2))/(32*M_1_PI*std::pow(h,9)));
    */
    /*return -glm::vec3(-(1890*(x1.x-x2.x)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)),
                     -(1890*(x1.y-x2.y)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)),
                     -(1890*(x1.z-x2.z)*std::pow((+std::pow((x2.x-x1.x),2)+std::pow((x2.y-x1.y),2)+std::pow((x2.z-x1.z),2)-h*h),2))/(M_1_PI*std::pow(h,9)));*/

    return ((float)(-45.0f/(M_1_PI*(std::pow(h,6))))*((float)std::pow((h-glm::length(x1-x2)),2))*glm::normalize(x2-x1));
}
