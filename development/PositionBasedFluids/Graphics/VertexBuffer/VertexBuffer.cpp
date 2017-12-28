#include"VertexBuffer.h"
#include<GL/glew.h>

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1,&id);
}

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,id);
}

void VertexBuffer::upload(const std::vector<Vertex>& vertices)
{
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),(void*)vertices.data(),GL_STATIC_DRAW);
}
