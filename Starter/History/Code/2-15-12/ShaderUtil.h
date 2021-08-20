#pragma once

#include <gl\glew.h>

#include <vector>

class ShaderUtil
{
public:
	ShaderUtil(void);
	~ShaderUtil(void);

	static bool LoadShaderFromString(char* fileName, std::vector<GLchar> &shaderCode);
	static bool CheckShaderCompile(GLuint shader);
	static GLuint CompileShader(char* fileName, GLuint shaderType);

	static void PrintAttributes(GLuint programHandle);
	static void PrintActiveUniformVars(GLuint programHandle);
};

