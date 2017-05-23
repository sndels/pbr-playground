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
    GLuint loadProgram(const std::string vertPath, const std::string fragPath,
                       const std::string geomPath);
    GLuint loadShader(const std::string& mainPath, GLenum shaderType);
    std::string parseFromFile(const std::string& filePath, GLenum shaderType);
    void printProgramLog(GLuint program) const;
    void printShaderLog(GLuint shader) const;

    GLuint                                  _progID;
    std::vector<std::vector<std::string> >  _filePaths;
    std::vector<std::vector<time_t> >       _fileMods;

};

#endif // SHADERPROGRAM_HPP
