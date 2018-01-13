#include"FrameBufferObject.h"
#include<GL/glew.h>

FrameBufferObject::FrameBufferObject()
{
    glCreateFramebuffers(1,&id);
}

FrameBufferObject::~FrameBufferObject()
{
    glDeleteFramebuffers(1,&id);
}

void FrameBufferObject::bind()
{
    glBindFramebuffer(id,GL_FRAMEBUFFER);
}

void unbind()
{
    glBindFramebuffer(0,GL_FRAMEBUFFER);
}

void FrameBufferObject::resize(unsigned int w,unsigned int h)
{
    glFramebufferParameteri(GL_FRAMEBUFFER,GL_FRAMEBUFFER_DEFAULT_WIDTH,w);
    glFramebufferParameteri(GL_FRAMEBUFFER,GL_FRAMEBUFFER_DEFAULT_HEIGHT,h);
}


void FrameBufferObject::attachColorImage(const Texture& image, unsigned int attNo)
{
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+attNo,image.id,0);
}

void FrameBufferObject::attachDepthImage(const Texture& image)
{
    glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,image.id,0);
}

void FrameBufferObject::attachStencilImage(const Texture& image)
{
    glFramebufferTexture(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,image.id,0);
}
