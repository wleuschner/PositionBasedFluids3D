#ifndef __VERTEX_BUFFER_H
#define __VERTEX_BUFFER_H
#include<vector>

class IndexBuffer
{
public:
    IndexBuffer();
    void bind();
    void upload();
private:
    unsigned int id;
};

#endif
