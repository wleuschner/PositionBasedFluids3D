#include"SpatialHashMap.h"

SpatialHashMap3D::SpatialHashMap3D(unsigned int size,float cellSize)
{
    this->size = size;
    this->cellSize = cellSize;
    buckets.resize(size);
    bucketLocks = new omp_lock_t[size];
    #pragma omp parallel for
    for(unsigned int i=0;i<size;i++)
    {
        omp_init_lock(&bucketLocks[i]);
    }
}

SpatialHashMap3D::~SpatialHashMap3D()
{
    #pragma omp parallel for
    for(unsigned int i=0;i<size;i++)
    {
        omp_destroy_lock(&bucketLocks[i]);
    }
}

void SpatialHashMap3D::insert(const Particle& p)
{

    unsigned int c = (((unsigned int)(std::floor(p.pos.x/cellSize)*73856093))^
                      ((unsigned int)(std::floor(p.pos.y/cellSize)*19349663))^
                      ((unsigned int)(std::floor(p.pos.z/cellSize)*83492791))) % size;
    buckets[c].push_back(p.index);
}

void SpatialHashMap3D::parallelInsert(const std::vector<Particle>& particles)
{
    #pragma omp parallel for
    for(unsigned int i=0;i<particles.size();i++)
    {
        Particle p = particles[i];
        unsigned int c = (((unsigned int)(std::floor(p.pos.x/cellSize)*73856093))^
                          ((unsigned int)(std::floor(p.pos.y/cellSize)*19349663))^
                          ((unsigned int)(std::floor(p.pos.z/cellSize)*83492791))) % size;
        omp_set_lock(&bucketLocks[c]);
        buckets[c].push_back(p.index);
        omp_unset_lock(&bucketLocks[c]);
    }
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
    nList.splice(nList.end(),bucket);
    //nList.insert(nList.end(),bucket.begin(),bucket.end());
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y-cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    //Center Layer
    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    //Top Layer
    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z-cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);

    c = (((unsigned int)(std::floor((p.pos.x-cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    c = (((unsigned int)(std::floor((p.pos.x+cellSize)/cellSize)*73856093))^
                      ((unsigned int)(std::floor((p.pos.y+cellSize)/cellSize)*19349663))^
                      ((unsigned int)(std::floor((p.pos.z+cellSize)/cellSize)*83492791))) % size;
    bucket = buckets[c];
    nList.splice(nList.end(),bucket);
    return nList;
}

void SpatialHashMap3D::clear()
{
    buckets.clear();
    buckets.resize(size);


}
