#include "GLSLProgram.h"

#include <fstream>
using std::ifstream;
using std::ios;

#include <sstream>
using std::ostringstream;

#include <sys/stat.h>

GLSLProgram::GLSLProgram() : handle(0), linked(false) { }

bool GLSLProgram::CompileShaderFromFile(const char * fileName,
                                        GLSLShader::GLSLShaderType type)
{
    if( ! FileExists(fileName) )
    {
        logString = "File not found.";
        return false;
    }

    if( handle <= 0 ) {
        handle = glCreateProgram();
        if( handle == 0) {
            logString = "Unable to create shader program.";
            return false;
        }
    }

    ifstream inFile( fileName, ios::in );
    if( !inFile ) {
        return false;
    }

    ostringstream code;
    while(inFile.good()) {
        int c = inFile.get();
        if( ! inFile.eof() ) code << (char) c;
    }
    inFile.close();

    return CompileShaderFromString(code.str(), type);
}

bool GLSLProgram::CompileShaderFromString( const string & source, GLSLShader::GLSLShaderType type)
{
    if( handle <= 0 ) {
        handle = glCreateProgram();
        if( handle == 0) {
            logString = "Unable to create shader program.";
            return false;
        }
    }

    GLuint shaderHandle = 0;

    switch( type ) {
    case GLSLShader::VERTEX:
        shaderHandle = glCreateShader(GL_VERTEX_SHADER);
        break;
    case GLSLShader::FRAGMENT:
        shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case GLSLShader::GEOMETRY:
        shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case GLSLShader::TESS_CONTROL:
        shaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
        break;
    case GLSLShader::TESS_EVALUATION:
        shaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
        break;
    default:
        return false;
    }

    const char * c_code = source.c_str();
    glShaderSource(shaderHandle, 1, &c_code, NULL);

    // Compile the shader
    glCompileShader(shaderHandle);

    // Check for errors
    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if( GL_FALSE == result ) {
        // Compile failed, store log and return false
        int length = 0;
        logString = "";
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
        if( length > 0 ) {
            char * c_log = new char[length];
            int written = 0;
            glGetShaderInfoLog(shaderHandle, length, &written, c_log);
            logString = c_log;
			printf("Error: %s\n", c_log);
            delete [] c_log;
        }

        return false;
    } else {
        // Compile succeeded, attach shader and return true
        glAttachShader(handle, shaderHandle);
        return true;
    }
}

bool GLSLProgram::Link()
{
    if( linked ) return true;
    if( handle <= 0 ) return false;

    glLinkProgram(handle);

    int status = 0;
    glGetProgramiv( handle, GL_LINK_STATUS, &status);
    if( GL_FALSE == status ) {
        // Store log and return false
        int length = 0;
        logString = "";

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length );

        if( length > 0 ) {
            char * c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete [] c_log;
        }

        return false;
    } else {
        linked = true;
        return linked;
    }
}

void GLSLProgram::Use()
{
    if( handle <= 0 || (! linked) ) return;
    glUseProgram( handle );
}

string GLSLProgram::Log()
{
    return logString;
}

int GLSLProgram::GetHandle()
{
    return handle;
}

bool GLSLProgram::IsLinked()
{
    return linked;
}

void GLSLProgram::BindAttribLocation(GLuint location, const char * name)
{
    glBindAttribLocation(handle, location, name);
}

void GLSLProgram::BindFragDataLocation(GLuint location, const char * name )
{
    glBindFragDataLocation(handle, location, name);
}

void GLSLProgram::SetUniform(const char *name, float x, float y, float z)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 ) {
        glUniform3f(loc,x,y,z);
    }
}

void GLSLProgram::SetUniform(const char *name, const vec3 & v)
{
    this->SetUniform(name,v.x,v.y,v.z);
}

void GLSLProgram::SetUniform(const char *name, const vec4 & v)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 ) {
        glUniform4f(loc,v.x,v.y,v.z,v.w);
    }
}

void GLSLProgram::SetUniform(const char *name, const mat4 & m)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    }
}

void GLSLProgram::SetUniform(const char *name, const mat3 & m)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
    }
}

void GLSLProgram::SetUniform(const char *name, float val)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1f(loc, val);
    }
}

void GLSLProgram::SetUniform(const char *name, int val)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    }
}

void GLSLProgram::SetUniform(const char *name, bool val)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    }
}

void GLSLProgram::PrintActiveUniforms() {

    GLint nUniforms, size, location, maxLen;
    GLchar * name;
    GLsizei written;
    GLenum type;

    glGetProgramiv( handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &nUniforms);

    name = (GLchar *) malloc( maxLen );

    printf(" Location | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( handle, i, maxLen, &written, &size, &type, name );
        location = glGetUniformLocation(handle, name);
        printf(" %-8d | %s\n",location, name);
    }

    free(name);
}

void GLSLProgram::PrintActiveAttribs() {

    GLint written, size, location, maxLength, nAttribs;
    GLenum type;
    GLchar * name;

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

    name = (GLchar *) malloc( maxLength );

    printf(" Index | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nAttribs; i++ ) {
        glGetActiveAttrib( handle, i, maxLength, &written, &size, &type, name );
        location = glGetAttribLocation(handle, name);
        printf(" %-5d | %s\n",location, name);
    }

    free(name);
}

int GLSLProgram::GetUniformLocation(const char * name)
{
    return glGetUniformLocation(handle, name);
}

bool GLSLProgram::FileExists(const string & fileName)
{
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}
