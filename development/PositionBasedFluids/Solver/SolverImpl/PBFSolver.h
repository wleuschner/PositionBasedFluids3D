#ifndef __PBF_SOLVER_H
#define __PBF_SOLVER_H
#include"../AbstractSolver.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialHashMap/SpatialHashMap.h"

class PBFSolver : AbstractSolver
{
public:
    PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations=4);
    PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations=4);


    void init(std::vector<Particle>& particles);
    void solve(std::vector<Particle>& particles);
private:

    std::vector<AbstractConstraint*> constraints;
    SpatialHashMap3D* spatialHashMap;
};

#endif
