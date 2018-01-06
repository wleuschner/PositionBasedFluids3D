#ifndef __RADIX_SORT_H
#define __RADIX_SORT_H
#include<atomic>
#include<vector>
#include<list>
#include<glm/glm.hpp>
#include"../AbstractSpatialStruct.h"
#include"../../Solver/Particle.h"
#include"../../Graphics/AABB/AABB.h"

class RadixSort : AbstractSpatialStruct
{
public:
    RadixSort(std::vector<Particle>& particles,float cellSize,const AABB& aabb);
    ~RadixSort();

    void update();
    std::list<unsigned int> find(const Particle &p);
    void clear();
    void setNewCellSize(float csize);

private:
    AABB aabb;
    glm::ivec3 dimSize;
    float cellSize;
    std::vector<unsigned int> sortedIndices;
    std::vector<unsigned int> histogram;
};
#endif
