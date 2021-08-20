#pragma once

#include "KScene.h"

#include <gl\glew.h>

class FuzzyCircle : public KScene
{
public:
	FuzzyCircle()
	{}
	virtual ~FuzzyCircle()
	{}

	void Init();
	void Render(float dt);

	void LinkShaders(GLint vertShader, GLint fragShader);
private:
	GLuint m_ProgramHandle;
	GLuint m_VaoHandle;
};

