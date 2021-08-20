#include "WireFrame.h"

#include "ShaderUtil.h"
#include "vbomesh.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;

WireFrame::WireFrame(void)
{	
	// initialize the ogre
    m_Ogre = new VBOMesh("mesh/bs_ears.obj");
}


WireFrame::~WireFrame(void)
{
	delete m_Ogre;
}

void WireFrame::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/wire_frame.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/wire_frame.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/wire_frame.geom", GLSLShader::GEOMETRY)) {
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

	m_Program.SetUniform("Line.Width", 0.25f);
	m_Program.SetUniform("Line.Color", 0.0f, 0.0f, 0.0f);

	m_Program.SetUniform("Light.Position", vec4(4.0f,10.0f,5.0f,1.0f));
	m_Program.SetUniform("Light.AmbientIntensity", 0.2f);
	m_Program.SetUniform("Light.DiffuseIntensity", 0.6f);
	m_Program.SetUniform("Light.SpecularIntensity", 0.6f);

	m_Program.SetUniform("Material.Ambient", 0.2f, 0.2f, 0.2f);
	m_Program.SetUniform("Material.Diffuse", 0.6f, 0.8f, 0.6f);
	m_Program.SetUniform("Material.Specular", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Material.Shininess", 1.0f);

}

void WireFrame::Motion(float x, float y)
{
	float dx = x - m_Mouse.Position.x;
	m_Mouse.SetPosition(x, m_Mouse.Position.y);
    m_Angle += dx;
    if( m_Angle > TWOPI) m_Angle -= TWOPI;
}

void WireFrame::Update(float dt)
{
    //m_Angle += dt / 10.0f;
    //if( m_Angle > TWOPI) m_Angle -= TWOPI;
}

void WireFrame::Resize(int width, int height)
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

void WireFrame::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos(1.0f * cos(m_Angle),0.0f,1.0f * sin(m_Angle));
	m_View = glm::lookAt(cameraPos,
                       vec3(0.0f,0.0f,0.0f),
                       vec3(0.0f,1.0f,0.0f));		
	mat4 modelView = m_View * m_Model;
	mat4 modelViewProjection = m_Projection * modelView;
	mat3 normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelView", modelView);
	m_Program.SetUniform("Projection", m_Projection);
	m_Program.SetUniform("MVP", modelViewProjection);
    m_Program.SetUniform("Viewport", m_Viewport);

    m_Model = mat4(1.0f);
    
	m_Ogre->render();

    glFinish();
}