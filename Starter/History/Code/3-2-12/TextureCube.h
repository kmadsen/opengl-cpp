#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include "GLSLProgram.h"

#include "KScene.h"
#include "vboteapot.h"

class TextureCube : public KScene
{
public:
	TextureCube(void)
		: m_Teapot(NULL)
	{}
	~TextureCube(void)
	{
		delete m_Teapot;
	}

	void Init();
	void Render(float dt);
	void Resize(int width, int height);
	void Idle() { }

private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;
	VBOTeapot *m_Teapot;

	mat4 m_ProjectionM;

	void LoadTextures();
};

#endif // TEXTURECUBE_H