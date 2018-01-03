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

    std::vector<AbstractConstraint*> constraints;
    SpatialHashMap3D* spatialHashMap;
};

#endif
