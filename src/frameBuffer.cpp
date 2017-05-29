#include "frameBuffer.hpp"

#include <iostream>

using std::cout;
using std::endl;

FrameBuffer::FrameBuffer(uint32_t w, uint32_t h, const std::vector<TextureParams>& texParams,
                         GLenum depthFormat, GLenum depthAttachment)
{
    // Generate and bind frame buffer object
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

    _texIDs.resize(texParams.size());
    glGenTextures(texParams.size(), _texIDs.data());
    for (auto i = 0u; i < texParams.size(); ++i) {
        // Generate texture
        glBindTexture(GL_TEXTURE_2D, _texIDs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, texParams[i].internalFormat, w, h, 0,
                     texParams[i].inputFormat, texParams[i].type, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texParams[i].minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texParams[i].magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParams[i].wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParams[i].wrapT);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Bind to fbo
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, _texIDs[i], 0);
    }

    if (depthFormat != 0 || depthAttachment != 0) {
        // Generate and bind depth buffer
        glGenRenderbuffers(1, &_depthRbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _depthRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthAttachment, GL_RENDERBUFFER, _depthRbo);
    } else
        cout << "[framebuffer] buffer generated without depth buffer" << endl;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "[framebuffer] Error creating framebuffer" << endl;
        cout << "Error code: " << error << endl;
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteTextures(_texIDs.size(), _texIDs.data());
    glDeleteFramebuffers(1, &_fbo);
    glDeleteRenderbuffers(1, &_depthRbo);
}

void FrameBuffer::bindWrite()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
}

void FrameBuffer::bindRead(const std::vector<GLenum>& texUnits, const std::vector<GLint>& uniforms)
{
    for (auto i = 0u; i < _texIDs.size(); ++i) {
        glActiveTexture(texUnits[i]);
        glBindTexture(GL_TEXTURE_2D, _texIDs[i]);
        glUniform1i(uniforms[i], texUnits[i] - GL_TEXTURE0);
    }
}

void FrameBuffer::genMipmap(uint32_t texNum)
{
    glBindTexture(GL_TEXTURE_2D, _texIDs.at(texNum));
    glGenerateMipmap(GL_TEXTURE_2D);
}
