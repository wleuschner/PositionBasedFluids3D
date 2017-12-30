#ifndef __PARTICLE_BUFFER_H_
#define __PARTICLE_BUFFER_H_
#include<vector>
#include"../../Solver/Particle.h"

class ParticleBuffer
{
public:
    ParticleBuffer();
    void bind();
    void addParticle(Particle particle);
    void upload();
    unsigned int getNumParticles();
    std::vector<Particle>& getParticles();
private:
    unsigned int id;
    std::vector<Particle> particles;
};

#endif