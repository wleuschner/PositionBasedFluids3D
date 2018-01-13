#ifndef __FRAMEBUFFEROBJECT_H
#define __FRAMEBUFFEROBJECT_H
#include"../Texture/Texture.h"

class FrameBufferObject
{
public:
    FrameBufferObject();
    ~FrameBufferObject();
    void bind();
    void unbind();
    void resize(unsigned int w,unsigned int h);

    void attachColorImage(const Texture& image, unsigned int attNo);
    void attachDepthImage(const Texture& image);
    void attachStencilImage(const Texture& image);
private:
    unsigned int id;
};

#endif
