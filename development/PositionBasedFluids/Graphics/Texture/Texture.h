#ifndef __TEXTURE_H
#define __TEXTURE_H

class Texture
{
    friend class FrameBufferObject;
public:
    Texture();
    ~Texture();

    void bind(unsigned int texUnit);
    void upload(unsigned int w,unsigned int h,void* data);
    void createDepthImage(unsigned int w,unsigned int h);

private:
    unsigned int id;
    unsigned int width;
    unsigned int height;
};

#endif
