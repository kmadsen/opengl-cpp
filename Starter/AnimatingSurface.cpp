#include "AnimatingSurface.h"

#include "PerspectiveProjection.h"
#include "ShaderUtil.h"
#include "vboplane.h"

#include <FreeImage.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec3;

static const float FOV = 35.0f;
static const float NEAR = 1.0f;
static const float FAR = 256.0f;


AnimatingSurface::AnimatingSurface(void)
{
	float aspectRatio = (float)m_Width/m_Height;
	m_CameraFrustum = new PerspectiveProjection(FOV, aspectRatio, NEAR, FAR);
    m_Plane = new VBOPlane(100.0f, 100.0f, 1000, 1000);
}


AnimatingSurface::~AnimatingSurface(void)
{
	delete m_CameraFrustum;
	delete m_Plane;
}


void AnimatingSurface::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/animating_surface.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/animating_surface.frag", GLSLShader::FRAGMENT)) {
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

	// Setup the rest of the scene
	m_Angle = 0.0;
	vec3 cameraPos(110.5f * cos(m_Angle),70.0f,110.5f * sin(m_Angle));
	m_CameraFrustum->SetView(cameraPos,vec3(0.0f),vec3(0.0f,1.0f,0.0f));

	// Initialize wave info
	m_Time = 0.0f;
	m_Amplitude = 10.0f;
	m_WaveLength = 20.8f;
	m_Velocity = 0.6f;

	m_Program.SetUniform("Time", m_Time);
	m_Program.SetUniform("Amplitude", m_Amplitude);
	m_Program.SetUniform("WaveLength", m_WaveLength);	
	m_Program.SetUniform("Velocity", m_Velocity);	
}

void AnimatingSurface::Update(float dt)
{
	m_Time += dt;
	//if (m_Time > TWOPI)
	//	m_Time = 0.0f;
	m_Program.SetUniform("Time", m_Time);
}

void AnimatingSurface::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 model = mat4(1.0f);
	
	mat4 view = m_CameraFrustum->GetViewMatrix();
	mat4 projection = m_CameraFrustum->GetPerspectiveMatrix();

	mat4 modelView = view * model;
	mat4 MVP = projection * modelView;
	mat3 normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelView", modelView);
	m_Program.SetUniform("MVP", MVP);

	m_Plane->render();
}


void AnimatingSurface::Resize(int width, int height)
{
	m_Width = width;
	m_Height = height;
	float aspectRatio = (float)m_Width/m_Height;
	m_CameraFrustum->SetPerspective(35.0f, aspectRatio, 1.0f, 256.0f);
	glViewport(0,0,m_Width,m_Height);
}


void AnimatingSurface::Motion(float x, float y)
{
	float dx = x - m_Mouse.Position.x;
	m_Mouse.SetPosition(x, m_Mouse.Position.y);
    m_Angle += dx;
    if( m_Angle > TWOPI) m_Angle -= TWOPI;

	vec3 cameraPos(110.5f * cos(m_Angle),70.0f,110.5f * sin(m_Angle));
	m_CameraFrustum->SetView(cameraPos,vec3(0.0f),vec3(0.0f,1.0f,0.0f));
}

