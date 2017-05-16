#include "shaderProgram.hpp"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

ShaderProgram::ShaderProgram(const std::string& vertPath, const std::string& fragPath,
                             const std::string& geomPath) :
    _progID(0),
    _vertPath(vertPath),
    _geomPath(geomPath),
    _fragPath(fragPath)
{
    GLuint progID = loadProgram();
    if (progID != 0) _progID = progID;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_progID);
}

bool ShaderProgram::isLinked() const
{
    return _progID != 0;
}

bool ShaderProgram::bind() const
{
    glUseProgram(_progID);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "[shader] Error binding shader program" << endl;
        cout << "Error code: " << error << endl;
        printProgramLog(_progID);
        return false;
    }
    return true;
}

void ShaderProgram::reload()
{
    GLuint progID = loadProgram();
    if (progID != 0) {
        glDeleteProgram(_progID);
        _progID = progID;
    }
}

GLint ShaderProgram::getULoc(const char* uniformName) const {
    GLint uniformLocation = glGetUniformLocation(_progID, uniformName);
    if (uniformLocation == -1) {
        cout << "[shader] " <<  uniformName << " is not a valid shader variable" << endl;
    }
    return uniformLocation;
}

GLuint ShaderProgram::loadProgram()
{
    GLuint progID = glCreateProgram();

    //Load and attacth shaders
    GLuint vertexShader = loadShaderFromFile(_vertPath, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        glDeleteProgram(progID);
        progID = 0;
        return 0;
    }
    glAttachShader(progID, vertexShader);

    GLuint geometryShader = 0;
    if (!_geomPath.empty()) {
        geometryShader = loadShaderFromFile(_geomPath, GL_GEOMETRY_SHADER);
        if (geometryShader == 0) {
            glDeleteShader(vertexShader);
            glDeleteProgram(progID);
            progID = 0;
            return 0;
        }
        glAttachShader(progID, geometryShader);
    }

    GLuint fragmentShader = loadShaderFromFile(_fragPath, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteProgram(progID);
        progID = 0;
        return 0;
    }
    glAttachShader(progID, fragmentShader);

    //Link program
    glLinkProgram(progID);
    GLint programSuccess = GL_FALSE;
    glGetProgramiv(progID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess == GL_FALSE) {
        cout << "[shader] Error linking program " << progID << endl;
        cout << "Error code: " << programSuccess;
        printProgramLog(_progID);
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(progID);
        progID = 0;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    cout << "[shader] Shader " << progID << " loaded" << endl;

    return progID;
}

GLuint ShaderProgram::loadShaderFromFile(const std::string& path, GLenum shaderType)
{
    GLuint shaderID = 0;
    std::ifstream sourceFile(path.c_str());
    if (sourceFile) {
        std::string shaderString((std::istreambuf_iterator<char>(sourceFile)),
                                  std::istreambuf_iterator<char>());
        shaderID = glCreateShader(shaderType);
        const GLchar* shaderSource = shaderString.c_str();
        glShaderSource(shaderID, 1, &shaderSource, NULL);
        glCompileShader(shaderID);
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
        if (shaderCompiled == GL_FALSE) {
            cout << "[shader] Unable to compile shader " << shaderID << endl;
            printShaderLog(shaderID);
            shaderID = 0;
        }
    } else {
        cout << "Unable to open file " << path << endl;
    }
    return shaderID;
}

void ShaderProgram::printProgramLog(GLuint program) const
{
    if (glIsProgram(program) == GL_TRUE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorLog = new char[maxLength];
        glGetProgramInfoLog(program, maxLength, &maxLength, errorLog);
        for (auto i = 0; i < maxLength; ++i)
            cout << errorLog[i];
        cout << endl;
        delete[] errorLog;
    } else {
        cout << "ID " << program << " is not a program" << endl;
    }
}

void ShaderProgram::printShaderLog(GLuint shader) const
{
    if (glIsShader(shader) == GL_TRUE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorLog = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
        for (auto i = 0; i < maxLength; ++i)
            cout << errorLog[i];
        cout << endl;
        delete[] errorLog;
    } else {
        cout << "ID " << shader << " is not a shader" << endl;
    }
}
