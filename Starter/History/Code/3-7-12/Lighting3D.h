#ifndef LIGHTING3D_H
#define LIGHTING3D_H

#include "GLSLProgram.h"

#include "KScene.h"
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
	void Resize(int width, int height);

private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;
	VBOTeapot *m_Teapot;

	mat4 m_ProjectionM;
};

#endif // LIGHTING3D_H