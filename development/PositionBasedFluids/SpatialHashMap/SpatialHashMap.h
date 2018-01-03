/*Spatial Hashmap from "Optimized Spatial Hashing for Collision Detection of Deformable Objects" by Teschner et. al.*/

#ifndef __SPATIAL_HASH_MAP_H
#define __SPATIAL_HASH_MAP_H
#include<vector>
#include<list>
#include<omp.h>
#include"../Solver/Particle.h"

class SpatialHashMap3D
{
public:
    SpatialHashMap3D(unsigned int size,float cellSize);
    ~SpatialHashMap3D();

    void parallelInsert(const std::vector<Particle>& particles);
    void insert(const Particle& p);
    std::list<unsigned int> find(const Particle &p);
    void clear();
    void setNewCellSize(float csize);

private:
    omp_lock_t* bucketLocks;
    unsigned int size;
    float tempCellSize;
    float cellSize;
    std::vector<std::list<unsigned int>> buckets;
};
#endif
