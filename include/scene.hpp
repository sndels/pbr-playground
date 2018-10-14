#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "shaderProgram.hpp"

class Scene
{
public:
    // Expects shaders to be vert, frag, (geom)
    Scene(const std::vector<std::string>& shaders, const std::vector<std::string>& uniforms);
    ~Scene() {}

    void bind();
    void reload();
    GLint getULoc(const std::string& uniform);

private:
    ShaderProgram                  _shaderProg;
    std::vector<std::string>       _uniforms;
    std::vector<GLint>             _uLocations;

};

#endif // SCENE_HPP
