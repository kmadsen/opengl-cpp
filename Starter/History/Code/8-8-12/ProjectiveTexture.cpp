#include "ProjectiveTexture.h"

#include "ShaderUtil.h"
#include "vboplane.h"
#include "vboteapot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;
using glm::vec3;


ProjectiveTexture::ProjectiveTexture()
{
	m_Plane = new VBOPlane(10.0f, 10.0f, 10, 10);
	m_Teapot = new VBOTeapot(10, mat4(1.0f));
}

ProjectiveTexture::~ProjectiveTexture()
{
	delete m_Teapot;
	delete m_Plane;
}

void ProjectiveTexture::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/projective_texture.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/projective_texture.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_Camera.SetEye(0.0f, 5.0f, 20.0f);
	m_Camera.SetCenter(0.0f,0.0f,0.0f);
	m_Camera.SetUp(0.0f,1.0f,0.0f);

	// Set up projector matrix
	vec3 projPos = vec3(2.0f, 5.0f, 5.0f);
	vec3 projAt = vec3(-2.0f, -4.0f, 0.0f);
	vec3 projUp = vec3(0.0f, 1.0f, 0.0f);

	mat4 projView = glm::lookAt(projPos, projAt, projUp);
	mat4 projProj = glm::perspective(30.0f, 1.0f, 0.2f, 1000.0f);
	mat4 projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(0.5f));
	mat4 projectorMatrix = projScaleTrans * projProj * projView;
	m_Program.SetUniform("ProjectorMatrix", projectorMatrix);


	m_Program.SetUniform("Light.Position", vec4(0.0f,5.0f,0.0f,1.0f) );
    m_Program.SetUniform("Light.Intensity", vec3(0.3f,0.3f,0.3f));

	LoadTextures();
}

void ProjectiveTexture::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionMatrix = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
	m_Program.SetUniform("Projection", m_ProjectionMatrix);
}

void ProjectiveTexture::SetMatrices()
{
	// set up matrices
	mat4 ModelView = m_ViewMatrix * m_ModelMatrix;
	mat4 MVP = m_ProjectionMatrix * ModelView;
	mat3 NormalMatrix = 
		mat3(vec3(ModelView[0]),
			 vec3(ModelView[1]),
			 vec3(ModelView[2]));

	m_Program.SetUniform("NormalMatrix", NormalMatrix);
	m_Program.SetUniform("ModelMatrix", m_ModelMatrix);
	m_Program.SetUniform("ModelViewMatrix", ModelView);
	m_Program.SetUniform("MVP", MVP);
}


void ProjectiveTexture::Render()
{
	// Clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	m_ModelMatrix = m_Rotater.GetTransform();
	m_ViewMatrix = m_Camera.GetView();

	m_Program.SetUniform("Material.Ambient", vec3(0.2f, 0.2f, 0.2f));
	m_Program.SetUniform("Material.Diffuse", vec3(1.0f, 0.0f, 0.0f));
	SetMatrices();
	m_Plane->render();
	
	m_Program.SetUniform("Material.Diffuse", vec3(0.0f, 1.0f, 0.0f));
	m_ModelMatrix = glm::rotate(m_ModelMatrix, -90.0f, 1.0f, 0.0f, 0.0f);
	SetMatrices();
	m_Teapot->render();
}

void ProjectiveTexture::Keyboard(int key, float x, float y)
{
	if (key == 'w') {
		m_Camera.MoveInViewDirection(0.1f);
	}
	else if (key == 's') {
		m_Camera.MoveInViewDirection(-0.1f);
	}

	printf("key %c pressed\n", key);
	m_Mouse.SetPosition(x, y);
}

void ProjectiveTexture::Idle()
{
}

void ProjectiveTexture::Mouse(const KMouse &mouse)
{
	// Copy the mouse data over
	m_Mouse = mouse;
}

void ProjectiveTexture::Motion(float x, float y)
{
	vec2 newPosition = vec2(x, y);
	vec2 delta = newPosition - m_Mouse.Position;
	m_Mouse.SetPosition(x, y);
	if (m_Mouse.Button == KMouse::LEFT) {
		m_Camera.Rotate(delta);
	}
	else if (m_Mouse.Button == KMouse::RIGHT) {
		m_Rotater.Rotate(delta);
	}
}

void ProjectiveTexture::LoadTextures()
{
	const static char* filename = "Textures/flower.png";

	GLuint tid;
	glGenTextures(1, &tid);

	// Load the diffuse texture
	FIBITMAP* imageBitmap = ShaderUtil::GetBitMap(filename);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(imageBitmap), 
		FreeImage_GetHeight(imageBitmap), 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(imageBitmap));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	m_Program.SetUniform("ProjectorTex", 0);
}
