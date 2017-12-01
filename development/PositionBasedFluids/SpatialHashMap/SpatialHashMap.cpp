#include"SpatialHashMap.h"

SpatialHashMap3D::SpatialHashMap3D(unsigned int size,float cellSize)
{
    this->size = size;
    this->cellSize = cellSize;
    buckets.reserve(size);
}

void SpatialHashMap3D::insert(const Particle& p)
{
    unsigned int c = (((unsigned int)(std::floor(p.pos.x/cellSize)*73856093))^
                      ((unsigned int)(std::floor(p.pos.y/cellSize)*19349663))^
                      ((unsigned int)(std::floor(p.pos.z/cellSize)*83492791))) % size;
    buckets[c].push_back(p);
}

std::list<Particle> SpatialHashMap3D::find(const Particle &p)
{
    unsigned int c = (((unsigned int)(std::floor(p.pos.x/cellSize)*73856093))^
                      ((unsigned int)(std::floor(p.pos.y/cellSize)*19349663))^
                      ((unsigned int)(std::floor(p.pos.z/cellSize)*83492791))) % size;
    return buckets[c];
}
