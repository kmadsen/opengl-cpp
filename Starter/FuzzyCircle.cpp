#include "FuzzyCircle.h"


#include <gl\glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ShaderUtil.h"

#include <stdio.h>

void FuzzyCircle::Init()
{
	glClearColor(0.2f,0.2f,0.2f,1.0f);

	// Compile a vertex shader
	GLuint vertShader = ShaderUtil::CompileShader("Shaders/basic_uniformblock.vert", GL_VERTEX_SHADER);

	// Compile a fragment shader
	GLuint fragShader = ShaderUtil::CompileShader("Shaders/basic_uniformblock.frag", GL_FRAGMENT_SHADER);

	// Link the shaders
	LinkShaders(vertShader, fragShader);

	// Get blob settings index from the shader
	GLuint blockIndex = glGetUniformBlockIndex(m_ProgramHandle, "BlobSettings");

	// Query for the offset of each variable
	const GLchar* names[] = { "InnerColor", "OuterColor", "RadiusInner", "RadiusOuter" };
	GLuint indices[4];
	glGetUniformIndices(m_ProgramHandle, 4, names, indices);
	GLint offset[4];
	glGetActiveUniformsiv(m_ProgramHandle, 4, indices, GL_UNIFORM_OFFSET, offset);

	// Allocate space for the block data
	GLint blockSize;
	glGetActiveUniformBlockiv(m_ProgramHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);

	// Put the data into the buffer at the offsets
	GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat innerRadius = 0.25f;
	GLfloat outerRadius = 0.45f;

	memcpy(blockBuffer + offset[0], innerColor, 4*sizeof(GLfloat));
	memcpy(blockBuffer + offset[1], outerColor, 4*sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
	memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));

	// Create the buffer object and copy the data into it
	GLuint uboHandle;
	glGenBuffers(1, &uboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

	// Bind the buffer object to the uniform block
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

	free(blockBuffer);
	blockBuffer = NULL;

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
	glGenVertexArrays(1, &m_VaoHandle );
	glBindVertexArray(m_VaoHandle);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );
}

void FuzzyCircle::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_VaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3 );
}

void FuzzyCircle::LinkShaders(GLint vertShader, GLint fragShader)
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