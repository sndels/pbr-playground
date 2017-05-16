#ifndef QUAD_HPP
#define QUAD_HPP

#include <GL/gl3w.h>

class Quad
{
public:
    Quad();
    ~Quad();

    void render() const;

private:
    GLuint _vao;
    GLuint _vbo;

};

#endif // QUAD_HPP
