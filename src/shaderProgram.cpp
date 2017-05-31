#include "shaderProgram.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <sys/stat.h>

using std::cout;
using std::endl;

// Get last time modified for file
time_t getMod(const std::string& path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == -1) {
        std::cout << "[shader] stat failed for " << path << std::endl;
        return (time_t) - 1;
    }

    return sb.st_mtime;
}

ShaderProgram::ShaderProgram(const std::string& vertPath, const std::string& fragPath,
                             const std::string& geomPath) :
    _progID(0),
    _filePaths(3),
    _fileMods(3)
{
    GLuint progID = loadProgram(vertPath, fragPath, geomPath);
    if (progID != 0) _progID = progID;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_progID);
}

void ShaderProgram::bind() const
{
    glUseProgram(_progID);
}

bool ShaderProgram::reload()
{
    // Reload shaders if some was modified
    for (auto j = 0u; j < 3; ++j) {
        for (auto i = 0u; i < _filePaths[j].size(); ++i) {
            if (_fileMods[j][i] != getMod(_filePaths[j][i])) {
                GLuint progID = loadProgram(_filePaths[1].size() > 0 ? _filePaths[1][0] : "",
                                            _filePaths[0].size() > 0 ? _filePaths[0][0] : "",
                                            _filePaths[2].size() > 0 ? _filePaths[2][0] : "");
                if (progID != 0) {
                    glDeleteProgram(_progID);
                    _progID = progID;
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}

GLint ShaderProgram::getULoc(const std::string& uniformName, bool debug) const {
    GLint uniformLocation = glGetUniformLocation(_progID, uniformName.c_str());
    if (debug && uniformLocation == -1) {
        cout << "[shader] " <<  uniformName << " is not a valid shader variable" << endl;
    }
    return uniformLocation;
}

GLuint ShaderProgram::loadProgram(const std::string vertPath, const std::string fragPath,
                                  const std::string geomPath)
{
    // Clear vectors
    for (auto& v : _filePaths) v.clear();
    for (auto& v : _fileMods) v.clear();

    // Get a program id
    GLuint progID = glCreateProgram();

    //Load and attacth shaders
    GLuint vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        glDeleteProgram(progID);
        progID = 0;
        return 0;
    }
    glAttachShader(progID, vertexShader);

    GLuint geometryShader = 0;
    if (!geomPath.empty()) {
        geometryShader = loadShader(geomPath, GL_GEOMETRY_SHADER);
        if (geometryShader == 0) {
            glDeleteShader(vertexShader);
            glDeleteProgram(progID);
            progID = 0;
            return 0;
        }
        glAttachShader(progID, geometryShader);
    }

    GLuint fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);
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

GLuint ShaderProgram::loadShader(const std::string& mainPath, GLenum shaderType)
{
    GLuint shaderID = 0;
    std::string shaderStr = parseFromFile(mainPath, shaderType);
    if (!shaderStr.empty()){
        shaderID = glCreateShader(shaderType);
        const GLchar* shaderSource = shaderStr.c_str();
        glShaderSource(shaderID, 1, &shaderSource, NULL);
        glCompileShader(shaderID);
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
        if (shaderCompiled == GL_FALSE) {
            cout << "[shader] Unable to compile shader " << shaderID << endl;
            printShaderLog(shaderID);
            shaderID = 0;
        }
    }
    return shaderID;
}

std::string ShaderProgram::parseFromFile(const std::string& filePath, GLenum shaderType)
{
    std::ifstream sourceFile(filePath.c_str());
    std::string shaderStr;
    if (sourceFile) {
        // Push filepath and timestamp to vectors
        if (shaderType == GL_FRAGMENT_SHADER) {
            _filePaths[0].emplace_back(filePath);
            _fileMods[0].emplace_back(getMod(filePath));
        } else if (shaderType == GL_VERTEX_SHADER) {
            _filePaths[1].emplace_back(filePath);
            _fileMods[1].emplace_back(getMod(filePath));
        } else {
            _filePaths[2].emplace_back(filePath);
            _fileMods[2].emplace_back(getMod(filePath));
        }

        // Get directory path for the file for possible includes
        std::string dirPath(filePath);
        dirPath.erase(dirPath.find_last_of('/') + 1);

        // Mark file start for error parsing
        shaderStr += "// File: " + filePath + '\n';

        // Parse lines
        for (std::string line; std::getline(sourceFile, line);) {
            // Handle recursive includes, expect correct syntax
            if (line.compare(0, 9, "#include ") == 0) {
                line.erase(0, 10);
                line.pop_back();
                line = parseFromFile(dirPath + line, shaderType);
                if (line.empty()) return "";
            }
            shaderStr += line + '\n';
        }

        // Mark file end for error parsing
        shaderStr += "// File: " + filePath + '\n';
    } else {
        cout << "Unable to open file " << filePath << endl;
    }
    return shaderStr;
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
        // Get errors to a stream
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorLog = new char[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
        std::istringstream errorStream(errorLog);

        // Get source string
        glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &maxLength);
        char* shaderStr = new char[maxLength];
        glGetShaderSource(shader, maxLength, &maxLength, shaderStr);

        std::string lastFile;
        // Parse correct file and line numbers to errors
        for (std::string errLine; std::getline(errorStream, errLine);) {

#ifdef _WIN32
            // Only parse if error points to a line
            if (errLine.compare(0, 2, "0(") == 0) {
                // Extract error line in parsed source
                auto lineNumEnd = errLine.find(')', 3);
                uint32_t lineNum = std::stoi(errLine.substr(2, lineNumEnd - 1));
#else
            if (errLine.compare(0, 9, "ERROR: 0:") == 0) {
                auto lineNumEnd = errLine.find(": ", 10);
                uint32_t lineNum = std::stoi(errLine.substr(9, lineNumEnd - 1));
#endif // _WIN32

                std::stack<std::string> files;
                std::stack<uint32_t> lines;
                // Parse the source to error, track file and line in file
                std::istringstream sourceStream(shaderStr);
                for (auto i = 0u; i < lineNum; ++i) {
                    std::string srcLine;
                    std::getline(sourceStream, srcLine);
                    if (srcLine.compare(0, 9, "// File: ") == 0) {
                        srcLine.erase(0, 9);
                        // If include-block ends, pop file and it's lines from stacks
                        if (!files.empty() && srcLine.compare(files.top()) == 0) {
                            files.pop();
                            lines.pop();
                        } else {// Push new file block to stacks
                            files.push(srcLine);
                            lines.push(0);
                        }
                    } else {
                        ++lines.top();
                    }
                }

                // Print the file if it changed from last error
                if (lastFile.empty() || lastFile.compare(files.top()) != 0) {
                    cout << endl << "In file " << files.top() << endl;
                    lastFile = files.top();
                }

#ifdef _WIN32
                // Insert the correct line number to error and print
                errLine.erase(2, lineNumEnd - 2);
                errLine.insert(2, std::to_string(lines.top()));
#else
                errLine.erase(9, lineNumEnd - 9);
                errLine.insert(9, std::to_string(lines.top()));
#endif // _WIN32

            }
            cout << errLine << endl;
        }
        cout << endl;

        delete[] errorLog;
        delete[] shaderStr;
    } else {
        cout << "ID " << shader << " is not a shader" << endl;
    }
}
