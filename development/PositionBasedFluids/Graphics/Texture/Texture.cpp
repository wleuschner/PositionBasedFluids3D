#include"Texture.h"
#include<GL/glew.h>

Texture::Texture()
{
    glGenTextures(1,&id);
}

Texture::~Texture()
{
    destroy();
}

void Texture::bind(unsigned int texUnit)
{
    glActiveTexture(GL_TEXTURE0+texUnit);
    glBindTexture(GL_TEXTURE_2D,id);
}

void Texture::upload(unsigned int w,unsigned int h,void* data)
{
    this->width = w;
    this->height = h;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
}

void Texture::createDepthImage(unsigned int w,unsigned int h)
{
    this->width = w;
    this->height = h;

    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,w,h,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
}

void Texture::destroy()
{
    glDeleteTextures(1,&id);
}
