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
    const glm::vec3 min = aabb.getCenter()-aabb.min;
    this->dimSize.x = glm::max(std::ceil(ext.x/cellSize),1.0f);
    this->dimSize.y = glm::max(std::ceil(ext.y/cellSize),1.0f);
    this->dimSize.z = glm::max(std::ceil(ext.z/cellSize),1.0f);

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
    std::vector<std::atomic<unsigned int>> ofs(elems);


    //Compute Bucket for Particle and count
    #pragma omp parallel for
    for(unsigned int i=0;i<particles.size();i++)
    {
        const Particle& p = particles[i];
        int xPos = glm::clamp((int)(std::floor((min.x+p.pos.x)/cellSize)),0,dimSize.x-1);
        int yPos = glm::clamp((int)(std::floor((min.y+p.pos.y)/cellSize)),0,dimSize.y-1);
        int zPos = glm::clamp((int)(std::floor((min.z+p.pos.z)/cellSize)),0,dimSize.z-1);

        int b = xPos+(dimSize.x*(yPos+dimSize.y*zPos));
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
    #pragma omp parallel for
    for(unsigned int i=0;i<particles.size();i++)
    {
        unsigned int bucket = particles[i].bucket;
        sortedIndices[histogram[bucket]+ofs[bucket].fetch_add(1)] = particles[i].index;
    }

}

std::list<unsigned int> RadixSort::find(const Particle &p)
{
    std::list<unsigned int> result;
    const glm::vec3 min = aabb.getCenter()-aabb.min;


    int xPos = glm::clamp((int)(std::floor((min.x+p.pos.x)/cellSize)),0,dimSize.x-1);
    int yPos = glm::clamp((int)(std::floor((min.y+p.pos.y)/cellSize)),0,dimSize.y-1);
    int zPos = glm::clamp((int)(std::floor((min.z+p.pos.z)/cellSize)),0,dimSize.z-1);

    unsigned int xPosBegin,xPosEnd;
    unsigned int yPosBegin,yPosEnd;
    unsigned int zPosBegin,zPosEnd;

    xPosBegin = std::max(xPos-1,0);
    xPosEnd   = std::min(xPos+1,dimSize.x-1);

    yPosBegin = std::max(yPos-1,0);
    yPosEnd   = std::min(yPos+1,dimSize.y-1);

    zPosBegin = std::max(zPos-1,0);
    zPosEnd   = std::min(zPos+1,dimSize.z-1);
    unsigned int nBucket = histogram.size();

    //Bottom
    {
        {
            unsigned int bottomLeftBack   = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*(zPosBegin)));
            unsigned int bottomRightBack   = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*(zPosBegin)));

            unsigned int beginIdx = histogram[bottomLeftBack];
            unsigned int endIdx = histogram[bottomRightBack+1];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        {
            unsigned int bottomLeftFront  = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*zPosEnd));
            unsigned int bottomRightFront  = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*zPosEnd));

            unsigned int beginIdx = histogram[bottomLeftFront];
            if(bottomRightFront==nBucket-1)
            {
                result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+particles.size());
            }
            else
            {
                unsigned int endIdx = histogram[bottomRightFront+1];
                result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
            }

        }
        unsigned int bottomLeftCenter = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*zPos));
        unsigned int bottomRightCenter = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*zPos));

        unsigned int beginIdx = histogram[bottomLeftCenter];
        unsigned int endIdx = histogram[bottomRightCenter+1];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }

    //Top
    {
        {
            unsigned int topLeftBack = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPosBegin));
            unsigned int topRightBack = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPosBegin));

            unsigned int beginIdx = histogram[topLeftBack];
            unsigned int endIdx = histogram[topRightBack+1];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }
        {
            unsigned int topLeftFront = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPosEnd));
            unsigned int topRightFront = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPosEnd));

            unsigned int beginIdx = histogram[topLeftFront];
            unsigned int endIdx = histogram[topRightFront+1];
            if(topRightFront==nBucket-1)
            {
                result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+particles.size());
            }
            else
            {
                unsigned int endIdx = histogram[topRightFront+1];
                result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
            }
        }
        unsigned int topLeftCenter = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPos));
        unsigned int topRightCenter = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPos));

        unsigned int beginIdx = histogram[topLeftCenter];
        if(topRightCenter==nBucket-1)
        {
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+particles.size());
        }
        else
        {
            unsigned int endIdx = histogram[topRightCenter+1];
            result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
        }


    }

    //Center
    {
        unsigned int centerLeftBack = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPosBegin));
        unsigned int centerRightBack = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPosBegin));

        unsigned int beginIdx = histogram[centerLeftBack];
        unsigned int endIdx = histogram[centerRightBack+1];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }
    {
        unsigned int centerLeftFront = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPosEnd));
        unsigned int centerRightFront = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPosEnd));

        unsigned int beginIdx = histogram[centerLeftFront];
        unsigned int endIdx = histogram[centerRightFront+1];
        result.insert(result.end(),sortedIndices.begin()+beginIdx,sortedIndices.begin()+endIdx);
    }
    unsigned int centerLeftCenter = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPos));
    unsigned int centerRightCenter = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPos));
    unsigned int beginIdx = histogram[centerLeftCenter];
    unsigned int endIdx = histogram[centerRightCenter+1];
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
