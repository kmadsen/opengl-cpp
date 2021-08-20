#include "CartoonShade.h"

#include "ShaderUtil.h"
#include "vboteapot.h"
#include "vboplane.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;

CartoonShade::~CartoonShade(void)
{
	delete m_Teapot;
}

void CartoonShade::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/cartoon_shade.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/cartoon_shade.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_Program.SetUniform("Light.Position", vec4(-20.0f,15.0f,40.0f,1.0f));
	m_Program.SetUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Material.Ambient", 0.1f, 0.1f, 0.1f);
	m_Program.SetUniform("Material.Diffuse", 0.7f, 0.1f, 0.1f);

	// initialize the teapot
	mat4 teapotLidTransform;
	teapotLidTransform = glm::translate(mat4(1.0f), vec3(-2,0,0));
	teapotLidTransform = glm::rotate(teapotLidTransform, -30.0f, vec3(0,1,0));
	teapotLidTransform = glm::translate(teapotLidTransform, vec3(2,0,0));
	
	m_Teapot = new VBOTeapot(20, teapotLidTransform);
	m_Plane = new VBOPlane(15, 15, 1, 1);
}

void CartoonShade::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionM = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
}

void CartoonShade::Render(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_View = lookAt(vec3(0,20,8), vec3(0,0,1), vec3(0,0,1));
	m_Model = mat4(1.0f);

	SetMatrices();

	m_Program.SetUniform("Material.Ambient", 0.1f, 0.1f, 0.1f);
	m_Program.SetUniform("Material.Diffuse", 0.7f, 0.1f, 0.1f);

	m_Teapot->render();

	m_Model = glm::rotate(m_Model, 90.0f, vec3(1.0f,0.0f,0.0f));
	SetMatrices();
	
	m_Program.SetUniform("Material.Ambient", 0.1f, 0.1f, 0.1f);
	m_Program.SetUniform("Material.Diffuse", 0.3f, 0.1f, 0.1f);

	m_Plane->render();

}

void CartoonShade::SetMatrices()
{
	mat4 ModelView = m_View * m_Model;
	mat4 MVP = m_ProjectionM * ModelView;
	mat3 NormalMatrix = 
		mat3(vec3(ModelView[0]),
			 vec3(ModelView[1]),
			 vec3(ModelView[2]));
	m_Program.SetUniform("NormalMatrix", NormalMatrix);
	m_Program.SetUniform("MVP", MVP);
}
