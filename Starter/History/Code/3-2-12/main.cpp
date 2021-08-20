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

//#include "BasicTriangle.h"
//#include "FuzzyCircle.h"
//#include "Lighting3D.h"
//#include "CartoonShade.h"
#include "TextureCube.h"

KScene *kScene;

int main_window = 1;
int position_x = 100;
int position_y = 100;
int width = 512;
int height = 512;


    
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

void display(void) {

	kScene->Render(0.0f);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0,0,w,h);

	kScene->Resize(w, h);

	glutPostRedisplay();
}

void idle()
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	kScene->Idle();

	// just keep redrawing the scene over and over
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

}

void mouse (int mouse, int state, int x, int y)
{

}

void motion(int x, int y)
{

}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100,100);
	main_window = glutCreateWindow("Assignment 1");
	glPointSize(1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
	}

	//DumpGLInfo(true);

	kScene = new TextureCube();

	kScene->Init();

	glutMainLoop();

	delete kScene;
}