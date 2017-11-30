/*Spatial Hashmap from "Optimized Spatial Hashing for Collision Detection of Deformable Objects" by Teschner et. al.*/

#ifndef __SPATIAL_HASH_MAP_H
#define __SPATIAL_HASH_MAP_H
#include<vector>
#include<list>
#include<glm/glm.hpp>

template<typename T> class SpatialHashMap;

template<> class SpatialHashMap<glm::vec3>
{
public:
    SpatialHashMap(unsigned int size,float cellSize)
    {
        this->size = size;
        this->cellSize = cellSize;
        buckets.reserve(size);
    }

    void insert(glm::vec3 pos)
    {
        unsigned int c = (((unsigned int)(std::floor(pos.x/cellSize)*73856093))^
                          ((unsigned int)(std::floor(pos.y/cellSize)*19349663))^
                          ((unsigned int)(std::floor(pos.z/cellSize)*83492791))) % size;
        buckets[c].push_back(pos);
    }

    std::list<glm::vec3> find(const glm::vec3 &pos)
    {
        unsigned int c = (((unsigned int)(std::floor(pos.x/cellSize)*73856093))^
                          ((unsigned int)(std::floor(pos.y/cellSize)*19349663))^
                          ((unsigned int)(std::floor(pos.z/cellSize)*83492791))) % size;
        return buckets[c];
    }

private:
    unsigned int size;
    float cellSize;
    std::vector<std::list<glm::vec3>> buckets;
};

template<> class SpatialHashMap<glm::vec2>
{
public:
    SpatialHashMap(unsigned int size,float cellSize)
    {
        this->size = size;
        this->cellSize = cellSize;
        buckets.reserve(size);
    }

    void insert(glm::vec2 pos)
    {
        unsigned int c = (((unsigned int)(std::floor(pos.x/cellSize)*73856093))^
                          ((unsigned int)(std::floor(pos.y/cellSize)*19349663))) % size;
        buckets[c].push_back(pos);
    }

    std::list<glm::vec2> find(const glm::vec2 &pos)
    {
        unsigned int c = (((unsigned int)(std::floor(pos.x/cellSize)*73856093))^
                          ((unsigned int)(std::floor(pos.y/cellSize)*19349663))) % size;
        return buckets[c];
    }

private:
    unsigned int size;
    float cellSize;
    std::vector<std::list<glm::vec2>> buckets;
};

#endif
