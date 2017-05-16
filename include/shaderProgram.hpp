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
    GLint getULoc(const char* uniformName) const;

private:
    GLuint loadProgram(const std::string& vertPath, const std::string& fragPath,
                       const std::string& geomPath);
    GLuint loadShaderFromFile(const std::string& path, GLenum shaderType);
    void printProgramLog(GLuint program) const;
    void printShaderLog(GLuint shader) const;

    GLuint _progID;

};

#endif // SHADERPROGRAM_HPP
