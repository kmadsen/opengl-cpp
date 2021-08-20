#ifndef CARTOONSHADE_H
#define CARTOONSHADE_H

#include "GLSLProgram.h"

#include "KScene.h"

class VBOTeapot;
class VBOPlane;

class CartoonShade : public KScene
{
public:
	CartoonShade(void)
		: m_Teapot(NULL)
	{}
	virtual ~CartoonShade(void);

	void Init();
	void Render();
	void Resize(int width, int height);

protected:
	void SetMatrices();

private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;

	VBOTeapot *m_Teapot;
	VBOPlane *m_Plane;

	mat4 m_ProjectionM;
	mat4 m_View;
	mat4 m_Model;
};



#endif // CARTOONSHADE_H