/*Spatial Hashmap from "Optimized Spatial Hashing for Collision Detection of Deformable Objects" by Teschner et. al.*/

#ifndef __SPATIAL_HASH_MAP_H
#define __SPATIAL_HASH_MAP_H
#include<vector>
#include<list>
#include"../Solver/Particle.h"

class SpatialHashMap3D
{
public:
    SpatialHashMap3D(unsigned int size,float cellSize);

    void insert(const Particle& p);
    std::list<Particle> find(const Particle &p);
    void clear();

private:
    unsigned int size;
    float cellSize;
    std::vector<std::list<Particle>> buckets;
};
#endif
