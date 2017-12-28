#include"IndexBuffer.h"
#include<GL/glew.h>

IndexBuffer::IndexBuffer()
{
    glGenBuffers(1,&id);
}

void IndexBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);
}
