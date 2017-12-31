#include"SpatialHashMap.h"

SpatialHashMap3D::SpatialHashMap3D(unsigned int size,float cellSize)
{
    this->size = size;
    this->cellSize = cellSize;
    buckets.resize(size);
}

void SpatialHashMap3D::insert(const Particle& p)
{
    unsigned int c = (((unsigned int)(std::floor(p.pos.x/cellSize)*73856093))^
                      ((unsigned int)(std::floor(p.pos.y/cellSize)*19349663))^
                      ((unsigned int)(std::floor(p.pos.z/cellSize)*83492791))) % size;
    buckets[c].push_back(p.index);
}

std::list<unsigned int> SpatialHashMap3D::find(const Particle &p)
{
    std::list<unsigned int> nList;
    std::list<unsigned int> bucket;
    unsigned int c = 0;

    //Bottom Layer
    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    //Center Layer
    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    //Top Layer
    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.insert(nList.end(),bucket.begin(),bucket.end());
    return nList;
}

void SpatialHashMap3D::clear()
{
    buckets.clear();
    buckets.resize(size);


}
