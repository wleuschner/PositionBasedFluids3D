#include "RadixSort.h"
#include <iostream>
#include <glm/glm.hpp>

RadixSort::RadixSort(std::vector<Particle>& particles,float cellSize,const AABB& aabb) : AbstractSpatialStruct(particles)
{
    this->cellSize = cellSize;
    this->aabb = aabb;
}

void RadixSort::update()
{
    const glm::vec3 ext = aabb.getExtent();
    const glm::vec3 min = aabb.min;
    this->dimSize.x = std::ceil(ext.x/cellSize);
    this->dimSize.y = std::ceil(ext.y/cellSize);
    this->dimSize.z = std::ceil(ext.z/cellSize);

    unsigned int elems = dimSize.x*dimSize.y*dimSize.z;
    if(elems!=histogram.size())
    {
        histogram.resize(elems);
    }
    if(particles.size()!=sortedIndices.size())
    {
        sortedIndices.resize(particles.size());
    }
    std::vector<std::atomic<unsigned int>> buckets(elems);
    std::vector<unsigned int> ofs(elems);


    //Compute Bucket for Particle and count
    #pragma omp parallel for
    for(unsigned int i=0;i<particles.size();i++)
    {
        const Particle& p = particles[i];
        int xPos = glm::clamp((int)(std::floor((min.x+p.pos.x)/cellSize)),0,dimSize.x);
        int yPos = glm::clamp((int)(std::floor((min.y+p.pos.y)/cellSize)),0,dimSize.y);
        int zPos = glm::clamp((int)(std::floor((min.z+p.pos.z)/cellSize)),0,dimSize.z);

        int b = xPos+(dimSize.y*(yPos+dimSize.x*zPos));
        //std::cout<<"EXT "<<ext.x<<" "<<ext.y<<" "<<ext.z<<std::endl;
        //std::cout<<"DIM "<<dimSize.x<<" "<<dimSize.y<<" "<<dimSize.z<<" "<<elems<<std::endl;
        //std::cout<<"POS "<<xPos<<" "<<yPos<<" "<<zPos<<" "<<b<<std::endl;
        particles[i].bucket = b;
        buckets[b]++;
    }

    //Compute Histogram
    unsigned int acc=0;
    for(unsigned int i=0;i<elems;i++)
    {
        histogram[i]=acc;
        acc+=buckets[i];
    }
    //ReInsert Particles
    for(unsigned int i=0;i<particles.size();i++)
    {
        unsigned int bucket = particles[i].bucket;
        sortedIndices[histogram[bucket]+ofs[bucket]] = particles[i].index;
        ofs[bucket]++;
    }

}

std::list<unsigned int> RadixSort::find(const Particle &p)
{
    std::list<unsigned int> result;
    const glm::vec3 min = aabb.min;


    int xPos = glm::clamp((int)(std::floor((min.x+p.pos.x)/cellSize)),0,dimSize.x);
    int yPos = glm::clamp((int)(std::floor((min.y+p.pos.y)/cellSize)),0,dimSize.y);
    int zPos = glm::clamp((int)(std::floor((min.z+p.pos.z)/cellSize)),0,dimSize.z);


    //Bottom
    if(yPos>0)
    {
        if(zPos>0)
        {
            unsigned int bottomLeftBack   = std::max(xPos-1,0)+(dimSize.y*(yPos-1+dimSize.x*(zPos-1)));
            unsigned int bottomRightBack   = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos-1+dimSize.x*(zPos-1)));

            unsigned int beginIdx = histogram[bottomLeftBack];
            unsigned int endIdx = histogram[bottomRightBack];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        if(zPos<dimSize.z-1)
        {
            unsigned int bottomLeftFront  = std::max(xPos-1,0)+(dimSize.y*(yPos-1+dimSize.x*(zPos+1)));
            unsigned int bottomRightFront  = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos-1+dimSize.x*(zPos+1)));

            unsigned int beginIdx = histogram[bottomLeftFront];
            unsigned int endIdx = histogram[bottomRightFront];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        unsigned int bottomLeftCenter = std::max(xPos-1,0)+(dimSize.y*(yPos-1+dimSize.x*zPos));
        unsigned int bottomRightCenter = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos-1+dimSize.x*zPos));

        unsigned int beginIdx = histogram[bottomLeftCenter];
        unsigned int endIdx = histogram[bottomRightCenter];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }

    //Top
    if(yPos<dimSize.y-1)
    {
        if(zPos>0)
        {
            unsigned int topLeftBack = std::max(xPos-1,0)+(dimSize.y*(yPos+1+dimSize.x*(zPos-1)));
            unsigned int topRightBack = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+1+dimSize.x*(zPos-1)));

            unsigned int beginIdx = histogram[topLeftBack];
            unsigned int endIdx = histogram[topRightBack];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        if(zPos<dimSize.z-1)
        {
            unsigned int topLeftFront = std::max(xPos-1,0)+(dimSize.y*(yPos+1+dimSize.x*(zPos+1)));
            unsigned int topRightFront = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+1+dimSize.x*(zPos+1)));

            unsigned int beginIdx = histogram[topLeftFront];
            unsigned int endIdx = histogram[topRightFront];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        unsigned int topLeftCenter = std::max(xPos-1,0)+(dimSize.y*(yPos+1+dimSize.x*zPos));
        unsigned int topRightCenter = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+1+dimSize.x*zPos));

        unsigned int beginIdx = histogram[topLeftCenter];
        unsigned int endIdx = histogram[topRightCenter];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);


    }

    //Center
    if(zPos>0)
    {
        unsigned int centerLeftBack = std::max(xPos-1,0)+(dimSize.y*(yPos+dimSize.x*(zPos-1)));
        unsigned int centerRightBack = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+dimSize.x*(zPos-1)));

        unsigned int beginIdx = histogram[centerLeftBack];
        unsigned int endIdx = histogram[centerRightBack];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }
    if(zPos<dimSize.z-1)
    {
        unsigned int centerLeftFront = std::max(xPos-1,0)+(dimSize.y*(yPos+dimSize.x*(zPos+1)));
        unsigned int centerRightFront = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+dimSize.x*(zPos+1)));

        unsigned int beginIdx = histogram[centerLeftFront];
        unsigned int endIdx = histogram[centerRightFront];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }
    unsigned int centerLeftCenter = std::max(xPos-1,0)+(dimSize.y*(yPos+dimSize.x*zPos));
    unsigned int centerRightCenter = std::min(xPos+1,dimSize.x-1)+(dimSize.y*(yPos+dimSize.x*zPos));
    unsigned int beginIdx = histogram[centerLeftCenter];
    unsigned int endIdx = histogram[centerRightCenter];
    result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);

    return result;

}

void RadixSort::clear()
{

}

void RadixSort::setNewCellSize(float csize)
{
    this->cellSize = csize;
}
