#include "ShadowMap.h"

#include "ShaderUtil.h"
#include "vboteapot.h"
#include "vbotorus.h"
#include "vboplane.h"


#include <FreeImage.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec3;

ShadowMap::ShadowMap(void)
{
    m_Teapot = new VBOTeapot(14, mat4(1.0f));
    m_Plane = new VBOPlane(40.0f, 40.0f, 4, 4);
    float scale = 2.0f;
    m_Torus = new VBOTorus(0.7f * scale,0.3f * scale,50,50);

	m_ShadowMapWidth = 1 << 9;
    m_ShadowMapHeight = 1 << 9;
}

ShadowMap::~ShadowMap(void)
{
	delete m_Teapot;
	delete m_Plane;
	delete m_Torus;
}

void ShadowMap::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/shadow_map.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/shadow_map.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	// This scales and translates the shadow coordinates to be in a 0 to 1 range
	m_ShadowBias = mat4(vec4(0.5f,0.0f,0.0f,0.0f),
						vec4(0.0f,0.5f,0.0f,0.0f),
						vec4(0.0f,0.0f,0.5f,0.0f),
						vec4(0.5f,0.5f,0.5f,1.0f)
						);

    float c = 1.65f;
	vec3 lightPos = vec3(0.0f, c* 5.25f, c* 7.5f);
	m_LightFrustum.SetView(lightPos, vec3(), vec3(0.0f, 1.0f, 0.0f));
	m_LightFrustum.SetPerspective(50.0f, 1.0f, 1.0f, 100.0f);
	m_LightPV = m_ShadowBias * m_LightFrustum.GetPerspectiveMatrix() * 
		m_LightFrustum.GetViewMatrix();

	m_Program.SetUniform("Light.Intensity", vec3(0.85f));
	m_Program.SetUniform("ShadowMap", 0);

	m_Pass1Index = glGetSubroutineIndex(m_Program.GetHandle(), GL_FRAGMENT_SHADER, "recordDepth");
    m_Pass2Index = glGetSubroutineIndex(m_Program.GetHandle(), GL_FRAGMENT_SHADER, "shadeWithShadow");

	m_Angle = 0;

	CreateShadowFBO();
}

void ShadowMap::Resize(int width, int height)
{
	m_Width = width;
	m_Height = height;
}

void ShadowMap::Render()
{
    // Set up camera perspective to render depth to a texture
	m_View = m_LightFrustum.GetViewMatrix();
	m_Projection = m_LightFrustum.GetPerspectiveMatrix();
    glViewport(0, 0, m_ShadowMapWidth, m_ShadowMapHeight);

	// Bind the frame buffer associated to the shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);

	// CLear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

	// Set the subroutine to record depth
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_Pass1Index);

	// Enable front face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

	// Draw the scene to the shadow map.
    DrawScene();

    glFlush();
    glFinish();
	//SaveDepthBuffer(); // Check to see if it's working...

	// Bind the viewing frame buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind the camera
	float c = 1.0f;
	vec3 cameraPos(c * 11.5f * cos(m_Angle),c * 7.0f,c * 11.5f * sin(m_Angle));
	m_View = glm::lookAt(cameraPos,vec3(0.0f),vec3(0.0f,1.0f,0.0f));
	m_Program.SetUniform("Light.Position", m_View * vec4(m_LightFrustum.GetOrigin(),1.0f));
	m_Projection = glm::perspective(50.0f, (float)m_Width/m_Height, 0.1f, 100.0f);
	SetMatrices();
	glViewport(0,0,m_Width,m_Height);

	// Disable culling (or we could switch to back-face culling)
	glDisable(GL_CULL_FACE);

	// Set the subroutine to shade with shadow
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_Pass2Index);

	DrawScene();

	glFinish();
}

void ShadowMap::Motion(float x, float y)
{
	float dx = x - m_Mouse.Position.x;
	m_Mouse.SetPosition(x, m_Mouse.Position.y);
    m_Angle += dx;
    if( m_Angle > TWOPI) m_Angle -= TWOPI;
}

void ShadowMap::SaveDepthBuffer()
{
    float * buffer = new float[m_ShadowMapWidth * m_ShadowMapHeight];
    glGetTexImage(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,GL_FLOAT,buffer);

	// Get min and max depths to have high contrast.
	float maxValue = 0.0f;
	float minValue = 1.0f;
	for( int i = 0; i < m_ShadowMapHeight; i++ ) {
        for( int j = 0; j < m_ShadowMapWidth; j++ ) {
			int bufIdx = i * m_ShadowMapWidth + j;
			float currentValue = buffer[bufIdx];
            
			maxValue = std::max(currentValue, maxValue);
			minValue = std::min(currentValue, minValue);
        }
	}
	float diff = maxValue - minValue;

	// Set to an image buffer.
    FIBITMAP *imgBuffer = FreeImage_Allocate(m_ShadowMapWidth, m_ShadowMapHeight, 24);
	RGBQUAD color;
    for( int i = 0; i < m_ShadowMapHeight; i++ ) {
        for( int j = 0; j < m_ShadowMapWidth; j++ ) {
			int bufIdx = i * m_ShadowMapWidth + j;
			float normValue = (buffer[bufIdx] - minValue) / diff;
            unsigned char val = (unsigned char)(normValue * 255);
			color.rgbRed = val;
			color.rgbGreen = val;
			color.rgbBlue = val;
			
			FreeImage_SetPixelColor(imgBuffer, j, i, &color); 
        }
	}

	// Save as a PNG.
	FreeImage_Save(FIF_PNG, imgBuffer, "depthBuffer.png");

    delete [] buffer;
    exit(1);
}

void ShadowMap::CreateShadowFBO()
{
	// Depth value of 1.0, this will be farthest possible (not a shadow).
	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

	// The shadow maptexture
	GLuint depthTex;

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		m_ShadowMapWidth, m_ShadowMapHeight, 0, 
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	
	// Assign the shadow map to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	// Create and set up the FBO
	glGenFramebuffers(1, &m_ShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = {GL_NONE};
	glDrawBuffers(1, drawBuffers);

	// Revert to the default framebuffer for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::DrawScene()
{
    vec3 color = vec3(0.7f,0.5f,0.3f);
    m_Program.SetUniform("Material.Ambient", color * 0.05f);
    m_Program.SetUniform("Material.Diffuse", color);
    m_Program.SetUniform("Material.Specular", vec3(0.9f,0.9f,0.9f));
    m_Program.SetUniform("Material.Shininess", 150.0f);
    m_Model = mat4(1.0f);
    m_Model *= glm::translate(vec3(0.0f,0.0f,0.0f));
    m_Model *= glm::rotate(-90.0f, vec3(1.0f,0.0f,0.0f));
    SetMatrices();
    m_Teapot->render();

    m_Program.SetUniform("Material.Ambient", color * 0.05f);
    m_Program.SetUniform("Material.Diffuse", color);
    m_Program.SetUniform("Material.Specular", vec3(0.9f,0.9f,0.9f));
    m_Program.SetUniform("Material.Shininess", 150.0f);
    m_Model = mat4(1.0f);
    m_Model *= glm::translate(vec3(0.0f,2.0f,5.0f));
    m_Model *= glm::rotate(-45.0f, vec3(1.0f,0.0f,0.0f));
    SetMatrices();
    m_Torus->render();

	m_Program.SetUniform("Material.Ambient", 0.05f, 0.05f, 0.05f);
    m_Program.SetUniform("Material.Diffuse", 0.25f, 0.25f, 0.25f);
    m_Program.SetUniform("Material.Specular", 0.0f, 0.0f, 0.0f);
    m_Program.SetUniform("Material.Shininess", 1.0f);
    m_Model = mat4(1.0f);
    m_Model *= glm::translate(vec3(0.0f,0.0f,0.0f));
    SetMatrices();
    m_Plane->render();
    m_Model = mat4(1.0f);
    m_Model *= glm::translate(vec3(-5.0f,5.0f,0.0f));
    m_Model *= glm::rotate(-90.0f,vec3(0.0f,0.0f,1.0f));
    SetMatrices();
    m_Plane->render();
    m_Model = mat4(1.0f);
    m_Model *= glm::translate(vec3(0.0f,5.0f,-5.0f));
    m_Model *= glm::rotate(90.0f,vec3(1.0f,0.0f,0.0f));
    SetMatrices();
    m_Plane->render();
    m_Model = mat4(1.0f);
}

void ShadowMap::SetMatrices()
{
	mat4 modelView = m_View * m_Model;
	mat4 modelViewProjection = m_Projection * modelView;
	mat3 normalMatrix = 
		mat3(vec3(modelView[0]),
			 vec3(modelView[1]),
			 vec3(modelView[2]));

	m_Program.SetUniform("NormalMatrix", normalMatrix);
	m_Program.SetUniform("ModelViewMatrix", modelView);
	m_Program.SetUniform("MVP", modelViewProjection);
	m_Program.SetUniform("ShadowMatrix", m_LightPV * m_Model);
}