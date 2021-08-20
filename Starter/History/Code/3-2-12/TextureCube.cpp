#include "TextureCube.h"

#include "ShaderUtil.h"
#include "vboteapot.h"
#include <FreeImage.h>
//#include <QImage>
//#include <QGLWidget>

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;

void TextureCube::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/texture_cube.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/texture_cube.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_Program.SetUniform("Light.Position", vec4(5.0f,5.0f,2.0f,1.0f));
	m_Program.SetUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Material.Ambient", 0.2f, 0.2f, 0.2f);
	m_Program.SetUniform("Material.Diffuse", 0.8f, 0.8f, 0.8f);

	// initialize the teapot
	mat4 teapotLidTransform;
	teapotLidTransform = glm::translate(mat4(1.0f), vec3(-2,0,0));
	teapotLidTransform = glm::rotate(teapotLidTransform, -30.0f, vec3(0,1,0));
	teapotLidTransform = glm::translate(teapotLidTransform, vec3(2,0,0));
	
	m_Teapot = new VBOTeapot(20, teapotLidTransform);

	LoadTextures();
}

void TextureCube::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionM = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
	m_Program.SetUniform("Projection", m_ProjectionM);
}

void TextureCube::Render(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 View = lookAt(vec3(0,10,8), vec3(0,0,1), vec3(0,0,1));
	mat4 Model = mat4(1.0f);
		
	mat4 ModelView = View * Model;
	mat4 MVP = m_ProjectionM * ModelView;
	mat3 NormalMatrix = 
		mat3(vec3(ModelView[0]),
			 vec3(ModelView[1]),
			 vec3(ModelView[2]));

	m_Program.SetUniform("NormalMatrix", NormalMatrix);
	m_Program.SetUniform("ModelView", ModelView);
	m_Program.SetUniform("MVP", MVP);

	m_Teapot->render();
}

void TextureCube::LoadTextures()
{
	char* textureFile = "Textures/brick1.jpg";
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(textureFile,0);
	FIBITMAP* imageBitmap = FreeImage_Load(formato, textureFile);
	
	// Convert to an OpenGL format and unload the old pointer
	FIBITMAP* temp = imageBitmap;
	imageBitmap = FreeImage_ConvertTo32Bits(imageBitmap);
	FreeImage_Unload(temp);
	
	int w = FreeImage_GetWidth(imageBitmap);
	int h = FreeImage_GetHeight(imageBitmap);
	
	char* pixels = reinterpret_cast<char*>(FreeImage_GetBits(imageBitmap));
	
	glActiveTexture(GL_TEXTURE0);
	GLuint tid;
	glGenTextures(1, &tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
		GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int loc = glGetUniformLocation(m_Program.GetHandle(), "Tex1");
	if (loc >= 0) {
		glUniform1i(loc, 0);
	}
	else {
		fprintf(stderr, "Uniform variable Tex1 not found.\n");
	}
}

