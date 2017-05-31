#ifndef SCENE_HPP
#define SCENE_HPP

#include <sync.h>
#include <vector>

#include "shaderProgram.hpp"

class Scene
{
public:
    // Expects shaders to be vert, frag, (geom)
    Scene(const std::vector<std::string>& shaders,
          const std::vector<std::string>& syncUniforms, sync_device* rocket);
    ~Scene() {}

    void bind(double syncRow);
    void reload();
    GLint getULoc(const std::string& uniform);

private:
    ShaderProgram                  _shaderProg;
    std::vector<std::string>       _uniforms;
    std::vector<GLint>             _uLocations;
    std::vector<const sync_track*> _syncTracks;

};

#endif // SCENE_HPP
