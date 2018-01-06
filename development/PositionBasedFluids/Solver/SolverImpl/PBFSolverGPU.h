#ifndef __PBF_SOLVER_GPU_H
#define __PBF_SOLVER_GPU_H
#include"../AbstractSolver.h"
#include"../Constraint/AbstractConstraint.h"
#include"../../SpatialStruct/SpatialHashMap/SpatialHashMap.h"
#include"../../Graphics/Shader/Shader.h"
#include"../../Graphics/ShaderProgram/ShaderProgram.h"


class PBFSolverGPU : AbstractSolver
{
public:
    PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,float timestep,int iterations=4);
    PBFSolverGPU(std::vector<Particle>& particles,AbstractKernel* densityKernel,AbstractKernel* gradKernel,AbstractKernel* viscKernel,std::vector<AbstractConstraint*> constraints,float timestep,int iterations=4);


    void init();
    void solve();


    void setDensityKernel(int id);
    void setGradKernel(int id);
    void setViscKernel(int id);

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
