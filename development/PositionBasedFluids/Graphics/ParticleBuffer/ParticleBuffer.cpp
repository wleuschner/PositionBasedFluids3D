#include"ParticleBuffer.h"
#include"../Vertex/Vertex.h"
#include<GL/glew.h>

ParticleBuffer::ParticleBuffer()
{
    glGenBuffers(1,&frontBufferId);
    glGenBuffers(1,&backBufferId);
    currBackBuffer = backBufferId;
}

void ParticleBuffer::bind()
{
    if(currBackBuffer!=frontBufferId)
    {
        glBindBuffer(GL_ARRAY_BUFFER,frontBufferId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,frontBufferId);
        glBindBuffer(GL_ARRAY_BUFFER,backBufferId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,backBufferId);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER,frontBufferId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,frontBufferId);
        glBindBuffer(GL_ARRAY_BUFFER,backBufferId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,backBufferId);
    }
    Vertex::enableVertexAttribs();
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)32);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)16);
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(2,1);
    glVertexAttribDivisor(3,1);

}

void ParticleBuffer::addParticle(Particle particle)
{
    particles.push_back(particle);
}

void ParticleBuffer::init()
{
    glBindBuffer(GL_ARRAY_BUFFER,frontBufferId);
    glBufferData(GL_ARRAY_BUFFER,particles.size()*sizeof(Particle),(void*)particles.data(),GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,backBufferId);
    glBufferData(GL_ARRAY_BUFFER,particles.size()*sizeof(Particle),(void*)particles.data(),GL_DYNAMIC_DRAW);
}

void ParticleBuffer::upload()
{
    glBindBuffer(GL_ARRAY_BUFFER,currBackBuffer);
    glBufferData(GL_ARRAY_BUFFER,particles.size()*sizeof(Particle),(void*)particles.data(),GL_DYNAMIC_DRAW);
}

void ParticleBuffer::swapBuffers()
{
    if(currBackBuffer!=frontBufferId)
    {
        currBackBuffer = frontBufferId;
    }
    else
    {
        currBackBuffer = backBufferId;
    }
}

unsigned int ParticleBuffer::getNumParticles()
{
    return particles.size();
}

std::vector<Particle>& ParticleBuffer::getParticles()
{
    return particles;
}
