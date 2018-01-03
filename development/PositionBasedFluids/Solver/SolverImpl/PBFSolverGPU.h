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

    void setNumIterations(unsigned int iterations);
    int getNumIterations();

    void setTimestep(float timestep);
    float getTimestep();

    void setKernelSupport(float support);
    float getKernelSupport();

    void setRestDensity(float density);
    float getRestDensity();

    void setArtVisc(float artVisc);
    float getArtVisc();

    void setArtVort(float artVort);
    float getArtVort();

    void setCfmRegularization(float cfm);
    float getCfmRegularization();

    void setCorrConst(float corrConst);
    float getCorrConst();

    void setCorrDist(float corrDist);
    float getCorrDist();

    void setCorrExp(float corrExp);
    float getCorrExp();
private:
    ShaderProgram* computeProgram;

    std::vector<AbstractConstraint*> constraints;
    SpatialHashMap3D* spatialHashMap;
};

#endif
