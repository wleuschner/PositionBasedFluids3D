#include"ParticleBuffer.h"
#include<GL/glew.h>

ParticleBuffer::ParticleBuffer()
{
    glGenBuffers(1,&id);
}

void ParticleBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,id);
}

void ParticleBuffer::addParticle(Particle particle)
{
    particles.push_back(particle);
}

void ParticleBuffer::upload()
{
    glBufferData(GL_ARRAY_BUFFER,particles.size()*sizeof(Particle),(void*)particles.data(),GL_DYNAMIC_DRAW);
}

void ParticleBuffer::clear()
{
    particles.clear();
}

unsigned int ParticleBuffer::getNumParticles()
{
    return particles.size();
}

std::vector<Particle>& ParticleBuffer::getParticles()
{
    return particles;
}
