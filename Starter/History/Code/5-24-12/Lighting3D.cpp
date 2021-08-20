#include "Lighting3D.h"

#include "ShaderUtil.h"
#include "vboteapot.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;

void Lighting3D::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/lighting3d.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/lighting3d.frag", GLSLShader::FRAGMENT)) {
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
	m_Program.SetUniform("Light.Ambient", 0.3f, 0.3f, 0.3f);
	m_Program.SetUniform("Light.Diffuse", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Light.Specular", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Material.Ambient", 0.7f, 0.1f, 0.1f);
	m_Program.SetUniform("Material.Diffuse", 0.7f, 0.1f, 0.1f);
	m_Program.SetUniform("Material.Specular", 0.8f, 0.8f, 0.8f);
	m_Program.SetUniform("Material.Shininess", 100.0f);

	// initialize the teapot
	mat4 teapotLidTransform;
	teapotLidTransform = glm::translate(mat4(1.0f), vec3(-2,0,0));
	teapotLidTransform = glm::rotate(teapotLidTransform, -30.0f, vec3(0,1,0));
	teapotLidTransform = glm::translate(teapotLidTransform, vec3(2,0,0));
	
	m_Teapot = new VBOTeapot(20, teapotLidTransform);
}

void Lighting3D::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionM = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
	m_Program.SetUniform("Projection", m_ProjectionM);
}

void Lighting3D::Render()
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

