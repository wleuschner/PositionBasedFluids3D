#include "AbstractKernel.h"

AbstractKernel::AbstractKernel(float h)
{
    this->h = h;
}

void AbstractKernel::setRadius(float h)
{
    this->h = h;
}

float AbstractKernel::getRadius()
{
    return h;
}
