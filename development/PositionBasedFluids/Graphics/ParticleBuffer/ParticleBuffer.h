#ifndef __PARTICLE_BUFFER_H_
#define __PARTICLE_BUFFER_H_
#include<vector>
#include"../../Solver/Particle.h"

class ParticleBuffer
{
public:
    ParticleBuffer();
    void bind();
    void upload(const std::vector<Particle>& vertices);
private:
    unsigned int id;
};

#endif
