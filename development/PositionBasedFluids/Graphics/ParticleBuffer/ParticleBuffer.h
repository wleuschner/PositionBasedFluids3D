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
    void init();
    void upload();
    void swapBuffers();
    unsigned int getNumParticles();
    std::vector<Particle>& getParticles();
private:
    unsigned int frontBufferId;
    unsigned int backBufferId;

    unsigned int currBackBuffer;
    std::vector<Particle> particles;
};

#endif
