#ifndef __ABSTRACT_SPATIAL_STRUCT_H
#define __ABSTRACT_SPATIAL_STRUCT_H
#include<vector>
#include<list>
#include<omp.h>
#include"../Solver/Particle.h"

class AbstractSpatialStruct
{
public:
    AbstractSpatialStruct(std::vector<Particle>& particles);
    ~AbstractSpatialStruct();

    virtual void update() = 0;
    virtual std::list<unsigned int> find(const Particle &p) = 0;
    virtual void clear() = 0;

    virtual void setNewCellSize(float csize) = 0;
protected:
    std::vector<Particle>& particles;
};
#endif
