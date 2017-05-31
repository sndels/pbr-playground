#include "scene.hpp"

Scene::Scene(const std::vector<std::string>& shaders,
             const std::vector<std::string>& syncUniforms, sync_device* rocket) :
    _shaderProg(shaders[0], shaders[1], shaders.size() > 2 ? shaders[2] : "")
{
    for (const auto& u : syncUniforms) {
        _uniforms.emplace_back(u);
        _uLocations.emplace_back(_shaderProg.getULoc(u));
        _syncTracks.emplace_back(sync_get_track(rocket, u.c_str()));
    }
}

GLint Scene::getULoc(const std::string& uniform)
{
    return _shaderProg.getULoc(uniform);
}

void Scene::bind(double syncRow)
{
    _shaderProg.bind();
    for (auto i = 0u; i < _uLocations.size(); ++i)
        glUniform1f(_uLocations[i], (float)sync_get_val(_syncTracks[i], syncRow));
}

void Scene::reload()
{
    if (_shaderProg.reload()) {
        _uLocations.clear();
        for (const auto& u : _uniforms)
            _uLocations.emplace_back(_shaderProg.getULoc(u));
   }
}
