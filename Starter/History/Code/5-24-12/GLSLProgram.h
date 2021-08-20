#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>
using std::string;

#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX, 
		FRAGMENT, 
		GEOMETRY,
        TESS_CONTROL, 
		TESS_EVALUATION
    };
};

class GLSLProgram
{
private:
    int handle;
    bool linked;
    string logString;

    int GetUniformLocation(const char* name);
    bool FileExists(const string & fileName);

public:
    GLSLProgram();

    bool CompileShaderFromFile(const char* fileName, GLSLShader::GLSLShaderType type);
    bool CompileShaderFromString(const string & source, GLSLShader::GLSLShaderType type);
    bool Link();
    void Use();

    string Log();

    int GetHandle();
    bool IsLinked();

    void BindAttribLocation(GLuint location, const char* name);
    void BindFragDataLocation(GLuint location, const char* name);

    void SetUniform(const char* name, float x, float y, float z);
    void SetUniform(const char* name, const vec3 & v);
    void SetUniform(const char* name, const vec4 & v);
    void SetUniform(const char* name, const mat4 & m);
    void SetUniform(const char* name, const mat3 & m);
    void SetUniform(const char* name, float val);
    void SetUniform(const char* name, int val);
    void SetUniform(const char* name, bool val);

    void PrintActiveUniforms();
    void PrintActiveAttribs();
};

#endif // GLSLPROGRAM_H
