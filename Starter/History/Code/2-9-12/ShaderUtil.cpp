#include "ShaderUtil.h"

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

ShaderUtil::ShaderUtil(void)
{
}


ShaderUtil::~ShaderUtil(void)
{
}

bool ShaderUtil::CheckShaderCompile(GLuint shader)
{
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);
			fprintf(stderr, "Shader log:\n%s", log);
			free(log);
		}
		return false;
	}
	return true;
}

GLuint ShaderUtil::CompileShader(char* fileName, GLuint shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		fprintf(stderr, "Error creating shader.\n");
		exit(1);
	}

	// Load the contents of the shader code
	std::vector<GLchar> shaderCode;
	if (!ShaderUtil::LoadShaderFromString(fileName, shaderCode)) {
		fprintf(stderr, "Error loading shader: %s\n", fileName);
		exit(1);
	}

	// Set the shader source
	const GLchar* codeArray[] = { &(shaderCode[0]) };
	glShaderSource(shader, 1, codeArray, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	if (!ShaderUtil::CheckShaderCompile(shader)) {
		fprintf(stderr, "Error compiling shader.\n");
		exit(1);
	}

	return shader;
}

bool ShaderUtil::LoadShaderFromString(char* fileName, vector<GLchar> &shaderCode)
{
	ifstream file;
	file.open(fileName, ios::in);
	if (file.is_open()) {
		file.seekg (0, ios::beg);
		int count = 0;
		while (file.good()) {
			int c = file.get();
			shaderCode.push_back(c);
			count++;
		}
		shaderCode[count-1] = 0;
		file.close();
		return true;
	}
	else {
		return false;
	}
}

void ShaderUtil::PrintAttributes(GLuint programHandle) 
{
	GLint maxLength, nAttribs;
	glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
	glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

	GLchar* name = (GLchar*)malloc(maxLength);

	GLint written, size, location;
	GLenum type;
	printf(" Index | Name\n");
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveAttrib(programHandle, i, maxLength, 
			&written, &size, &type, name);
		location = glGetAttribLocation(programHandle, name);
		printf(" %-5d | %s\n", location, name);
	}

	free(name);
}