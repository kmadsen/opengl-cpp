#pragma once

#include "KScene.h"

#include <gl\glew.h>

class BasicTriangle : public KScene
{
public:
	BasicTriangle()
	{
		m_Angle = 0.0f;
	}
	virtual ~BasicTriangle()
	{}

	void Init();
	void Render();
	
	void LinkShaders(GLint vertShader, GLint fragShader);

private:
	GLuint m_ProgramHandle;
	GLuint m_VaoHandle;
	float m_Angle;
};

