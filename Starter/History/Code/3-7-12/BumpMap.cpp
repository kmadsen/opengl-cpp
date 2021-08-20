#include "BumpMap.h"

#include "ShaderUtil.h"
#include "vbomesh.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;

BumpMap::BumpMap()
	: m_Ogre(NULL)
{
	// initialize the ogre
    m_Ogre = new VBOMesh("mesh/bs_ears.obj");
}

BumpMap::~BumpMap()
{
	delete m_Ogre;
}

void BumpMap::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/bump_map.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/bump_map.frag", GLSLShader::FRAGMENT)) {
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

	m_Camera.SetEye(0.0f,0.0f,5.0f);
	m_Camera.SetCenter(0.0f,0.0f,0.0f);
	m_Camera.SetUp(0.0f,1.0f,0.0f);

	LoadTextures();
}

void BumpMap::Resize(int width, int height)
{
	float aspectRatio = width / (float)height;
	m_ProjectionM = perspective(35.0f, aspectRatio, 0.1f, 100.0f);
	m_Program.SetUniform("Projection", m_ProjectionM);
}

void BumpMap::Render(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	mat4 ModelView = m_Camera.GetView() * m_Rotater.GetTransform();
	mat4 MVP = m_ProjectionM * ModelView;
	mat3 NormalMatrix = 
		mat3(vec3(ModelView[0]),
			 vec3(ModelView[1]),
			 vec3(ModelView[2]));

	m_Program.SetUniform("NormalMatrix", NormalMatrix);
	m_Program.SetUniform("ModelView", ModelView);
	m_Program.SetUniform("MVP", MVP);

	m_Ogre->render();
}

void BumpMap::Keyboard(int key, float x, float y)
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

void BumpMap::Mouse(const KMouse &mouse)
{
	// Copy the mouse data over
	m_Mouse = mouse;
}

void BumpMap::Motion(float x, float y)
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

FIBITMAP* BumpMap::GetBitMap(char const * const filename) const
{
	FIBITMAP* imageBitmap = FreeImage_Load(FreeImage_GetFileType(filename,0), filename);
	FIBITMAP* temp = imageBitmap;
	imageBitmap = FreeImage_ConvertTo32Bits(imageBitmap);
	FreeImage_Unload(temp);

	return imageBitmap;
}

void BumpMap::LoadTextures()
{
	const static char* filenames[] = {
		"Textures/ogre_diffuse.png",
		"Textures/ogre_normalmap.png"
	};

	GLuint tid[2];
	glGenTextures(2, tid);

	// Load the diffuse texture
	FIBITMAP* imageBitmap = GetBitMap(filenames[0]);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tid[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(imageBitmap), 
		FreeImage_GetHeight(imageBitmap), 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(imageBitmap));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_Program.SetUniform("ColorTex", 0);

	// Load the normal map
	imageBitmap = GetBitMap(filenames[1]);
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tid[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(imageBitmap), 
		FreeImage_GetHeight(imageBitmap), 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(imageBitmap));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_Program.SetUniform("NormalTex", 1);
}

