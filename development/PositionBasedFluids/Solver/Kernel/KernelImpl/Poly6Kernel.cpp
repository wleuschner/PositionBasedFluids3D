#include"Poly6Kernel.h"
#include<cmath>

Poly6Kernel::Poly6Kernel(float h) : AbstractKernel(h)
{

}

float Poly6Kernel::execute(float r)
{
    return (315.0/(64*M_1_PI*std::pow(h,9)))*std::pow((h*h-r*r),3);
}
