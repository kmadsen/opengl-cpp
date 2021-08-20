#include "Tessellation.h"

#include "ShaderUtil.h"
#include "vboteapotpatch.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;

Tessellation::Tessellation(void)
{
	m_Teapot = new VBOTeapotPatch();
}

Tessellation::~Tessellation(void)
{
	delete m_Teapot;
}


void Tessellation::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/tessellation.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/tessellation.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/tessellation.geom", GLSLShader::GEOMETRY)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/tessellation.tcs", GLSLShader::TESS_CONTROL)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/tessellation.tes", GLSLShader::TESS_EVALUATION)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_Angle = 0.0f;
	m_TessLevel = 4;

    ///////////// Uniforms //////////////////////
	//m_Program.SetUniform("TessLevel", m_TessLevel);
    m_Program.SetUniform("LineWidth", 0.8f);
    m_Program.SetUniform("LineColor", vec4(0.05f,0.0f,0.05f,1.0f));
    m_Program.SetUniform("LightPosition", vec4(0.0f,0.0f,0.0f,1.0f));
    m_Program.SetUniform("LightIntensity", vec3(1.0f,1.0f,1.0f));
    m_Program.SetUniform("Kd", vec3(0.9f, 0.9f, 1.0f));

    m_Program.SetUniform("MinTessLevel", 2);
    m_Program.SetUniform("MaxTessLevel", 20);
    m_Program.SetUniform("MaxDepth", 15.0f);
    m_Program.SetUniform("MinDepth", 5.0f);
    /////////////////////////////////////////////
}

void Tessellation::Motion(float x, float y)
{
	float dx = x - m_Mouse.Position.x;
	m_Mouse.SetPosition(x, m_Mouse.Position.y);
    m_Angle += dx + dx;
    if( m_Angle > TWOPI) m_Angle -= static_cast<float>(TWOPI);
}

void Tessellation::Keyboard(int key, float x, float y)
{
	if (key == 'u' || key == 'U') {
		m_TessLevel++;
	}
	else if (key == 'd' || key == 'D') {
		m_TessLevel--;
	}
	//m_Program.SetUniform("TessLevel", m_TessLevel);
	m_Mouse.SetPosition(x, y);
}

void Tessellation::Resize(int width, int height)
{
    glViewport(0,0,width,height);
    m_Projection = glm::perspective(50.0f, (float)width/height, 0.3f, 100.0f);

	float w2 = width / 2.0f;
    float h2 = height / 2.0f;
    m_Viewport = mat4( vec4(w2,0.0f,0.0f,0.0f),
                     vec4(0.0f,h2,0.0f,0.0f),
                     vec4(0.0f,0.0f,1.0f,0.0f),
                     vec4(w2+0, h2+0, 0.0f, 1.0f));
}

void Tessellation::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float zDist = 8.0f;
    vec3 cameraPos(9,2.0f, 3);
	m_View = glm::lookAt(cameraPos,
                       vec3(5.5f,2.0f,0.0f),
                       vec3(0.0f,1.0f,0.0f));		

    m_Model = glm::rotate(mat4(1.0f), m_Angle*360, vec3(0.0f, 1.0f, 0.0f));
	m_Model = glm::rotate(m_Model, -90.0f, vec3(1.0f, 0.0f, 0.0f));
    
	mat4 modelView = m_View * m_Model;
	mat4 modelViewProjection = m_Projection * modelView;
	mat3 normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelView", modelView);
	m_Program.SetUniform("MVP", modelViewProjection);
    m_Program.SetUniform("Viewport", m_Viewport);
	
	m_Teapot->render();

	m_Model = glm::translate(m_Model, vec3(2, 8, 0));

	modelView = m_View * m_Model;
	modelViewProjection = m_Projection * modelView;
	normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelView", modelView);
	m_Program.SetUniform("MVP", modelViewProjection);

	m_Teapot->render();

	m_Model = glm::translate(m_Model, vec3(3, 16, 0));

	modelView = m_View * m_Model;
	modelViewProjection = m_Projection * modelView;
	normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelView", modelView);
	m_Program.SetUniform("MVP", modelViewProjection);

	m_Teapot->render();

    glFinish();
}