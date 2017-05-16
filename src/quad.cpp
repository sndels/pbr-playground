#include "quad.hpp"

Quad::Quad() :
    _vao(0),
    _vbo(0)
{
    // Generate arrays
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);

    // Upload vertex data
    GLfloat verts[18] = {-1.f, -1.f, 0.f,
                          1.f, -1.f, 0.f,
                          1.f,  1.f, 0.f,
                          1.f,  1.f, 0.f,
                         -1.f,  1.f, 0.f,
                         -1.f, -1.f, 0.f };
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, verts, GL_STATIC_DRAW);

    // Set vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Unbinds for safety, vao first!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Quad::~Quad()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void Quad::render() const
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
