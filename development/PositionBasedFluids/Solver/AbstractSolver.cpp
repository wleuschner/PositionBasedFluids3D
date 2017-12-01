#include"AbstractSolver.h"

AbstractSolver::AbstractSolver()
{
}

void AbstractSolver::setTimestep(float timestep)
{
    this->timestep = timestep;
}

float AbstractSolver::getTimestep()
{
    return timestep;
}
