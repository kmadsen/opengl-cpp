#include "PointSprites.h"

#include "ShaderUtil.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::perspective;
using glm::lookAt;
using glm::mat4;
using glm::vec2;

using std::rand;
using std::srand;
#include <ctime>
using std::time;


PointSprites::PointSprites(void)
{
}


PointSprites::~PointSprites(void)
{
}

void PointSprites::Init()
{
	// Compile and link shaders
	if (!m_Program.CompileShaderFromFile("Shaders/point_sprite.vert", GLSLShader::VERTEX)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/point_sprite.frag", GLSLShader::FRAGMENT)) {
		exit(1);
	}
	if (!m_Program.CompileShaderFromFile("Shaders/point_sprite.geom", GLSLShader::GEOMETRY)) {
		exit(1);
	}
	if (!m_Program.Link()) {
		exit(1);
	}
	m_Program.Use();

	// Initialize gl
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glEnable(GL_DEPTH_TEST);

	m_NumSprites = 50;
    m_Locations = new float[m_NumSprites * 3];

    srand( time(0) );
    for( int i = 0; i < m_NumSprites; i++ ) {
        vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
               ((float)rand() / RAND_MAX * 2.0f) - 1.0f,
               ((float)rand() / RAND_MAX * 2.0f) - 1.0f);
        m_Locations[i*3] = p.x;
        m_Locations[i*3+1] = p.y;
        m_Locations[i*3+2] = p.z;
    }
	m_Angle = 0.0f;

	// Set up the buffers
    GLuint handle;
    glGenBuffers(1, &handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, m_NumSprites * 3 * sizeof(float), m_Locations, GL_STATIC_DRAW);

    delete [] m_Locations;

	    // Set up the vertex array object
    glGenVertexArrays( 1, &m_Sprites );
    glBindVertexArray(m_Sprites);

    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);

    // Load texture file
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

	m_Program.SetUniform("SpriteTexture", 0);
    m_Program.SetUniform("Size2", 0.15f);
}

void PointSprites::Update(float dt)
{
    m_Angle += dt / 10.0f;
    if( m_Angle > TWOPI) m_Angle -= TWOPI;
}

void PointSprites::Resize(int width, int height)
{
    glViewport(0,0,width,height);
    m_Projection = glm::perspective(60.0f, (float)width/height, 0.3f, 100.0f);
}

void PointSprites::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos(3.0f * cos(m_Angle),0.0f,3.0f * sin(m_Angle));
    m_View = glm::lookAt(cameraPos,
                       vec3(0.0f,0.0f,0.0f),
                       vec3(0.0f,1.0f,0.0f));

    m_Model = mat4(1.0f);
    
	mat4 mv = m_View * m_Model;
	m_Program.SetUniform("ModelViewMatrix", mv);
    m_Program.SetUniform("ProjectionMatrix", m_Projection);

    glBindVertexArray(m_Sprites);
    glDrawArrays(GL_POINTS, 0, m_NumSprites);

    glFinish();
}