#include"ViscocityKernel.h"
#include<cmath>

ViscocityKernel::ViscocityKernel(float h) : AbstractKernel(h)
{

}

float ViscocityKernel::execute(const glm::vec3& r)
{
    if(glm::length(r)>h)
    {
        return 0;
    }
    return -(std::pow(glm::length(r),3)/(2*std::pow(h,3)))-(std::pow(glm::length(r),2)/(std::pow(h,2)))+(h/(2*glm::length(r)))-1;
}

glm::vec3 ViscocityKernel::grad(const glm::vec3& r)
{
    if(glm::length(r)>h || glm::length(r)<0.0001)
    {
        return glm::vec3(0.0,0.0,0.0);
    }
    return glm::vec3(0.0,0.0,0.0);
}


float ViscocityKernel::laplacian(const glm::vec3 &r)
{
    return (45/(M_1_PI*std::pow(h,6)))*(h-glm::length(r));
}
