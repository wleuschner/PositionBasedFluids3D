#ifndef __PBF_SOLVER_GPU_H
#define __PBF_SOLVER_GPU_H
#include"../AbstractSolver.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialHashMap/SpatialHashMap.h"
#include"../../Graphics/Shader/Shader.h"
#include"../../Graphics/ShaderProgram/ShaderProgram.h"


class PBFSolverGPU : AbstractSolver
{
public:
    PBFSolverGPU(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations=4);
    PBFSolverGPU(AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations=4);


    void init(std::vector<Particle>& particles);
    void solve(std::vector<Particle>& particles);
private:
    ShaderProgram* computeProgram;

    std::vector<AbstractConstraint*> constraints;
    SpatialHashMap3D* spatialHashMap;
};

#endif
