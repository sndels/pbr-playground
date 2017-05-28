#include "texture.hpp"

#include <iostream>

using std::cout;
using std::endl;

Texture::Texture(uint32_t w, uint32_t h, TextureParams params) :
    _texID(0),
    _params(params)
{
    glGenTextures(1, &_texID);
    glBindTexture(GL_TEXTURE_2D, _texID);
    glTexImage2D(GL_TEXTURE_2D, 0, params.internalFormat, w, h, 0,
                 params.inputFormat, params.type, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);


    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "[texture] Error creating texture" << endl;
        cout << "Error code: " << error << endl;
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &_texID);
}

void Texture::bindWrite(GLenum attach)
{
    glFramebufferTexture(GL_FRAMEBUFFER, attach, _texID, 0);
}

void Texture::bindRead(GLenum texUnit, GLint uniform)
{
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, _texID);
    glUniform1i(uniform, texUnit - GL_TEXTURE0);
}

void Texture::resize(uint32_t w, uint32_t h)
{
    glBindTexture(GL_TEXTURE_2D, _texID);
    glTexImage2D(GL_TEXTURE_2D, 0, _params.internalFormat, w, h, 0,
                 _params.inputFormat, _params.type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "[texture] Error resizing texture" << endl;
        cout << "Error code: " << error << endl;
    }
}

void Texture::genMipmap()
{
    glGenerateTextureMipmap(_texID);
}
