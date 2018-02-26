#include"ParticleBuffer.h"
#include<GL/glew.h>

ParticleBuffer::ParticleBuffer()
{
    glGenBuffers(1,&id);
}

ParticleBuffer::~ParticleBuffer()
{
    glDeleteBuffers(1,&id);
}

void ParticleBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,id);
}

void ParticleBuffer::addParticle(Particle particle)
{
    particle.index = getNumParticles()+1;
    particles.push_back(particle);
}

void ParticleBuffer::syncGPU()
{
    bind();
    Particle* data = (Particle*)glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
    unsigned int parts = particles.size();
    particles.clear();
    for(unsigned int i=0;i<parts;i++)
    {
        particles.push_back(data[i]);
    }
    upload();
}

void ParticleBuffer::merge(const ParticleBuffer& b2)
{
    unsigned int offset = getNumParticles();
    bind();
    for(unsigned int i=0;i<b2.particles.size();i++)
    {
        Particle p = b2.particles[i];
        p.index = offset+i;
        addParticle(p);
    }
    upload();
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
