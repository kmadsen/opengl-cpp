#include "BasicTriangle.h"

#include <gl\glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ShaderUtil.h"

#include <stdio.h>

void BasicTriangle::Init()
{
	glClearColor(0.2f,0.2f,0.2f,1.0f);

	// Compile a vertex shader
	GLuint vertShader = ShaderUtil::CompileShader("Shaders/basic_uniform.vert", GL_VERTEX_SHADER);

	// Compile a fragment shader
	GLuint fragShader = ShaderUtil::CompileShader("Shaders/basic_uniform.frag", GL_FRAGMENT_SHADER);

	// Link the shaders
	LinkShaders(vertShader, fragShader);

	/////////////////// Create the VBO ////////////////////
	float positionData[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f,  0.8f, 0.0f };
	float colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };

	// Create and populate the buffer objects
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

	// Create and set-up the vertex array object
	glGenVertexArrays( 1, &m_VaoHandle );
	glBindVertexArray(m_VaoHandle);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );
}

void BasicTriangle::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	m_Angle = m_Angle <= 359.0f ? m_Angle+0.01f : 0.0f;
	
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_Angle, glm::vec3(0.0f,0.0f,1.0f));
	GLuint location = glGetUniformLocation(m_ProgramHandle, "RotationMatrix");
	if (location >= 0) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
	}

    glBindVertexArray(m_VaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3 );
}

void BasicTriangle::LinkShaders(GLint vertShader, GLint fragShader)
{
	// Create the program
	m_ProgramHandle = glCreateProgram();
	if (m_ProgramHandle == 0) {
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}

	/* Instead of doing this, change basic.vert to have something different */
	//// Bind index 0 to the shader input variable "VertexPosition"
	//glBindAttribLocation(programHandle, 0, "VertexPosition");
	//// Bind index 1 to the shader input variable "VertexColor"
	//glBindAttribLocation(programHandle, 1, "VertexColor");

	/* This is automatic, so we don't need to do it */
	//glBindFragDataLocation(programHandle, 0, "FragColor");

	// Attach the shaders to the program object
	glAttachShader(m_ProgramHandle, vertShader);
	glAttachShader(m_ProgramHandle, fragShader);

	// Link the program
	glLinkProgram(m_ProgramHandle);

	// Verify the link status
	GLint status;
	glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		fprintf(stderr, "Failed to link shader program.\n");
		GLint logLen;
		glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(m_ProgramHandle, logLen, &written, log);
			fprintf(stderr, "Program log: \n%s", log);
			free(log);
		}
	}
	else {
		// Install the program into the OpenGL pipeline
		glUseProgram(m_ProgramHandle);
	}

	// Print information about the active attributes in the shader program
	ShaderUtil::PrintAttributes(m_ProgramHandle);
}