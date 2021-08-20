#include "EnvironmentMap.h"

#include "ShaderUtil.h"
#include "skybox.h"
#include "vboteapot.h"
#include "vbotorus.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;

EnvironmentMap::EnvironmentMap()
	: m_SkyBox(NULL)
{
	// initialize the models
    m_SkyBox = new SkyBox();
	m_Teapot = new VBOTeapot(10, mat4(1.0f));
	m_Torus = new VBOTorus(1.5f, 0.7f, 100, 100);
}

EnvironmentMap::~EnvironmentMap()
{
	delete m_Torus;
	delete m_Teapot;
	delete m_SkyBox;
}

void EnvironmentMap::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/environment_map.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/environment_map.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_Program.SetUniform("Light.Position", vec4(4.0f,10.0f,5.0f,1.0f));
	m_Program.SetUniform("Light.Ambient", 0.2f, 0.2f, 0.2f);
	m_Program.SetUniform("Light.Diffuse", 0.8f, 0.8f, 0.8f);
	m_Program.SetUniform("Light.Specular", 0.2f, 0.2f, 0.2f);

	m_Program.SetUniform("Material.Specular", 1.0f, 1.0f, 1.0f);
	m_Program.SetUniform("Material.Shininess", 1.0f);

	m_Camera.SetEye(0.0f,-3.0f,15.0f);
	m_Camera.SetCenter(0.0f,0.0f,0.0f);
	m_Camera.SetUp(0.0f,-1.0f,0.0f);

	LoadCubeMap();
}

void EnvironmentMap::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionMatrix = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
	m_Program.SetUniform("Projection", m_ProjectionMatrix);
}

void EnvironmentMap::SetMatrices()
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
	m_Program.SetUniform("ProjectionMatrix", m_ProjectionMatrix);
	m_Program.SetUniform("MVP", MVP);
}


void EnvironmentMap::Render()
{
	// Clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	/* Render the skybox */
	
	// Disable the depth and make the sky box model folow the camera.
	// This will make the sky box appear infinately far away no matter where
	// the camera moves
	glDisable(GL_DEPTH_TEST);
	m_Program.SetUniform("DrawSkyBox", true);
	
	m_ViewMatrix = m_Camera.GetView();
	
	m_ModelMatrix = glm::translate(mat4(1.0), m_Camera.GetEye());
	
	SetMatrices();
	
	m_SkyBox->render();


	/* Render the teapot */

	glEnable(GL_DEPTH_TEST);
	m_Program.SetUniform("DrawSkyBox", false);
	m_Program.SetUniform("WorldCameraPosition", m_Camera.GetEye());
	m_Program.SetUniform("Material.ReflectionFactor", 1.0f);
	
	m_ViewMatrix = m_Camera.GetView();
	m_ViewMatrix = glm::translate(m_ViewMatrix, vec3(6.0f, 1.0f, 0.0f));

	m_ModelMatrix = m_Rotater.GetTransform();
	m_ModelMatrix = glm::rotate(m_ModelMatrix, 90.0f, vec3(1.0f, 0.0f, 0.0f));

	SetMatrices();
	
	m_Teapot->render();


	/* Render the torus */

	m_Program.SetUniform("Material.ReflectionFactor", 0.2f);
	m_Program.SetUniform("Material.Eta", 0.96f);
	m_ViewMatrix = m_Camera.GetView();

	m_ModelMatrix = m_Rotater.GetTransform();
	m_ModelMatrix = glm::rotate(m_ModelMatrix, 90.0f, vec3(1.0f, 0.0f, 0.0f));

	SetMatrices();
	m_Teapot->render();
}

void EnvironmentMap::Keyboard(int key, float x, float y)
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

void EnvironmentMap::Idle()
{
	//m_Rotater.Rotate(vec2(0.0001f, 0.0f));
}

void EnvironmentMap::Mouse(const KMouse &mouse)
{
	// Copy the mouse data over
	m_Mouse = mouse;
}

void EnvironmentMap::Motion(float x, float y)
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

FIBITMAP* EnvironmentMap::GetBitMap(char const * const filename) const
{
	FIBITMAP* imageBitmap = FreeImage_Load(FreeImage_GetFileType(filename,0), filename);
	FIBITMAP* temp = imageBitmap;
	imageBitmap = FreeImage_ConvertTo32Bits(imageBitmap);
	FreeImage_Unload(temp);

	return imageBitmap;
}

void EnvironmentMap::LoadCubeMap()
{
	glActiveTexture(GL_TEXTURE0);

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);

	//const char* baseFileName = "Textures/cubemap_panorama/";
	//const char* extension = ".png";
	const char* baseFileName = "textures/cubemap_snow/";
	const char* extension = ".jpg";
	const char* suffixes[] =  {
		"posx", "negx", 
		"negy", "posy", 
		"posz", "negz"
	};

	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		string texName = string(baseFileName) + suffixes[i] + extension;
		FIBITMAP* imageBitmap = GetBitMap(texName.c_str());
		glTexImage2D(targets[i], 0, GL_RGBA, 
			FreeImage_GetWidth(imageBitmap), FreeImage_GetHeight(imageBitmap), 
			0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(imageBitmap));
	}

	// Set the cube map settings
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_Program.SetUniform("CubeMapTex", 0);
}

