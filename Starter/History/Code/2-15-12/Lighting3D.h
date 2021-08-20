#pragma once

#include "KScene.h"

#include "GLSLProgram.h"

#include "vboteapot.h"

class Lighting3D : public KScene
{
public:
	Lighting3D(void)
		: m_Teapot(NULL)
	{}
	~Lighting3D(void)
	{
		delete m_Teapot;
	}

	void Init();
	void Render(float dt);
	void Idle() { }

private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;
	VBOTeapot *m_Teapot;


};

