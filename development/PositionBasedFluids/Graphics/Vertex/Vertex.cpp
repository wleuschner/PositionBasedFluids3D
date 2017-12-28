#include"Vertex.h"
#include<GL/glew.h>

void Vertex::enableVertexAttribs()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void Vertex::setVertexAttribs()
{
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,24,(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,24,(void*)12);
}
