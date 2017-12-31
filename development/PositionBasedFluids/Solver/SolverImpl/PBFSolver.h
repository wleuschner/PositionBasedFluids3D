#ifndef __PBF_SOLVER_H
#define __PBF_SOLVER_H
#include"../AbstractSolver.h"
#include"../Kernel/AbstractKernel.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialHashMap/SpatialHashMap.h"

class PBFSolver : AbstractSolver
{
public:
    PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations=4);
    PBFSolver(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations=4);

    void setNumIterations(unsigned int iterations);
    int getNumIterations();

    void init(std::vector<Particle>& particles);
    void solve(std::vector<Particle>& particles);
private:
    unsigned int iterations;
    float kernelSupport;
    float restDensity;
    float artVisc;
    float cfmRegularization;
    float corrConst;
    float corrDist;
    float corrExp;

    std::vector<AbstractConstraint*> constraints;
    AbstractKernel* densityKernel;
    AbstractKernel* gradKernel;
    AbstractKernel* viscKernel;
    SpatialHashMap3D* spatialHashMap;
};

#endif
