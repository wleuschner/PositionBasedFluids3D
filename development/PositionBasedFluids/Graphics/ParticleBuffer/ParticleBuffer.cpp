#include"ParticleBuffer.h"
#include<GL/glew.h>

ParticleBuffer::ParticleBuffer()
{
    glGenBuffers(1,&id);
}

void ParticleBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,id);
}

void ParticleBuffer::upload(const std::vector<Particle>& vertices)
{
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Particle),(void*)vertices.data(),GL_STREAM_DRAW);
}
