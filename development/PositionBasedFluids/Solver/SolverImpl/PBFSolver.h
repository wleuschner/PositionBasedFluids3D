#ifndef __PBF_SOLVER_H
#define __PBF_SOLVER_H
#include"../AbstractSolver.h"

class PBFSolver : AbstractSolver
{
public:
    PBFSolver(float timestep);
    void solve(std::vector<Particle>& particles);
private:
    unsigned int iterations;
};

#endif
