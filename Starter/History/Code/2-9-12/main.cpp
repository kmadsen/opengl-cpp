// Good instructions to begin a OpenGL project - http://openglbook.com/setting-up-opengl-glew-and-freeglut-in-visual-c/#toc-step-1-get-the-libraries

#include <gl\glew.h>
#include <gl\glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ShaderUtil.h"

#include <stdio.h>

int main_window = 1;
int position_x = 100;
int position_y = 100;
int width = 512;
int height = 512;
    
GLuint vaoHandle;
GLuint programHandle;

void DumpGLInfo(bool dumpExtensions) {
    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    printf("GL Vendor    : %s\n", vendor);
    printf("GL Renderer  : %s\n", renderer);
    printf("GL Version   : %s\n", version);
    printf("GL Version   : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);

    if( dumpExtensions ) {
        GLint nExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
        for( int i = 0; i < nExtensions; i++ ) {
            printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
        }
    }
}

void matrixExample()
{
	glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 view = glm::lookAt( 
		glm::vec3(0.0,0.0,5.0),
		glm::vec3(0.0,0.0,0.0),
		glm::vec3(0.0,1.0,0.0) );
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, 90.0f, glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 modelView = view * model;
	glm::vec4 transformed = modelView * position;
}

void LinkShaders(GLint vertShader, GLint fragShader)
{
	// Create the program
	programHandle = glCreateProgram();
	if (programHandle == 0) {
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
	glAttachShader(programHandle, vertShader);
	glAttachShader(programHandle, fragShader);

	// Link the program
	glLinkProgram(programHandle);

	// Verify the link status
	GLint status;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		fprintf(stderr, "Failed to link shader program.\n");
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen, &written, log);
			fprintf(stderr, "Program log: \n%s", log);
			free(log);
		}
	}
	else {
		// Install the program into the OpenGL pipeline
		glUseProgram(programHandle);
	}

	// Print information about the active attributes in the shader program
	ShaderUtil::PrintAttributes(programHandle);
}

void Init() {

	glClearColor(0.2f,0.2f,0.2f,1.0f);

	// Compile a vertex shader
	GLuint vertShader = ShaderUtil::CompileShader("Shaders/basic.vert", GL_VERTEX_SHADER);

	// Compile a fragment shader
	GLuint fragShader = ShaderUtil::CompileShader("Shaders/basic.frag", GL_FRAGMENT_SHADER);

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
	glGenVertexArrays( 1, &vaoHandle );
	glBindVertexArray(vaoHandle);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 0, (GLubyte *)NULL );
}

float angle = 0.0f;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	angle = angle <= 359.0f ? angle+0.01f : 0.0f;
	
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f,0.0f,1.0f));
	GLuint location = glGetUniformLocation(programHandle, "RotationMatrix");
	if (location >= 0) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);
	}

    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3 );

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void idle(void)
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	// just keep redrawing the scene over and over
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100,100);
	main_window = glutCreateWindow("Assignment 1");
	glPointSize(1);

	glutDisplayFunc(display);
	//glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
	}

	//DumpGLInfo(true);

	Init();

	glutMainLoop();
}