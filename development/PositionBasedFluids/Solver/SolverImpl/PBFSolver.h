#ifndef __PBF_SOLVER_H
#define __PBF_SOLVER_H
#include"../AbstractSolver.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialHashMap/SpatialHashMap.h"

class PBFSolver : AbstractSolver
{
public:
    PBFSolver(float timestep);
    void solve(std::vector<Particle>& particles);
private:
    unsigned int iterations;
    float kernelSupport;

    std::vector<AbstractConstraint*> constraints;
    //SpatialHashMap<glm::vec3> spatialHashMap;
};

#endif
