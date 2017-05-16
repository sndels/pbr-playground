#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <GL/gl3w.h>
#include <string>
#include <vector>

class ShaderProgram
{
public:
    ShaderProgram(const std::string& vertPath, const std::string& fragPath,
                  const std::string& geomPath = std::string());
    ~ShaderProgram();

    bool isLinked() const;
    bool bind() const;
    void reload();
    GLint getULoc(const char* uniformName) const;

private:
    GLuint loadProgram();
    GLuint loadShaderFromFile(const std::string& path, GLenum shaderType);
    void printProgramLog(GLuint program) const;
    void printShaderLog(GLuint shader) const;

    GLuint _progID;
    std::string _vertPath;
    std::string _geomPath;
    std::string _fragPath;
    time_t      _vertMod;
    time_t      _geomMod;
    time_t      _fragMod;

};

#endif // SHADERPROGRAM_HPP
