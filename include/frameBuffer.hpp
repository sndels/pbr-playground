#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <GL/gl3w.h>
#include <vector>

#include "texture.hpp"

class FrameBuffer
{
public:
    FrameBuffer(uint32_t w, uint32_t h, const std::vector<TextureParams>& texParams,
                GLenum depthFormat = 0, GLenum depthAttachment = 0);
    ~FrameBuffer();

    void bindWrite();
    void bindRead(const std::vector<GLenum>& texUnits, const std::vector<GLint>& uniforms);
    void genMipmap(uint32_t texNum);

private:
    GLuint               _fbo;
    std::vector<GLuint>  _texIDs;
    GLuint               _depthRbo;

};

#endif // FRAMEBUFFER_HPP
