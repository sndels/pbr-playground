#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/gl3w.h>
#include <vector>

struct TextureParams
{
    GLenum internalFormat;
    GLenum inputFormat;
    GLenum type;
    GLenum minFilter;
    GLenum magFilter;
    GLenum wrapS;
    GLenum wrapT;
};

class Texture
{
public:
    Texture(uint32_t w, uint32_t h, TextureParams params);
    ~Texture();

    void bindWrite(GLenum attach);
    void bindRead(GLenum texUnit, GLint uniform);
    void resize(uint32_t w, uint32_t h);
    void genMipmap();

private:
    GLuint        _texID;
    TextureParams _params;

};

#endif // TEXTURE_HPP
