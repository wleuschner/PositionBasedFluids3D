#ifndef __PBF_SOLVER_H
#define __PBF_SOLVER_H
#include"../AbstractSolver.h"
#include"../Kernel/AbstractKernel.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialHashMap/SpatialHashMap.h"

class PBFSolver : AbstractSolver
{
public:
    PBFSolver(AbstractKernel* kernel,float timestep);
    PBFSolver(AbstractKernel* kernel,float timestep,int iterations);
    PBFSolver(AbstractKernel* kernel,std::vector<AbstractConstraint*> constraints,float timestep);
    PBFSolver(AbstractKernel* kernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations);

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
    AbstractKernel* kernel;
    SpatialHashMap3D* spatialHashMap;
};

#endif
