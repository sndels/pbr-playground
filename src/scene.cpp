#include "scene.hpp"

Scene::Scene(const std::vector<std::string>& shaders,
             const std::vector<std::string>& uniforms) :
    _shaderProg(shaders[0], shaders[1], shaders.size() > 2 ? shaders[2] : "")
{
    for (const auto& u : uniforms) {
        _uniforms.emplace_back(u);
        _uLocations.emplace_back(_shaderProg.getULoc(u));
    }
}

GLint Scene::getULoc(const std::string& uniform)
{
    return _shaderProg.getULoc(uniform);
}

void Scene::bind()
{
    _shaderProg.bind();
}

void Scene::reload()
{
    if (_shaderProg.reload()) {
        _uLocations.clear();
        for (const auto& u : _uniforms)
            _uLocations.emplace_back(_shaderProg.getULoc(u));
   }
}
