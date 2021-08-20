#ifndef TESSELLATION_H
#define TESSELLATION_H

#include "kscene.h"
#include "GLSLProgram.h"

class VBOTeapotPatch;

class Tessellation : public KScene
{
public:
	Tessellation(void);
	virtual ~Tessellation(void);

	void Init() override;
	void Render() override;
	void Resize(int width, int height) override;
	void Mouse(const KMouse &mouse) override { m_Mouse = mouse; }
	void Motion(float x, float y) override;
	void Keyboard(int key, float x, float y) override;
private:
	GLSLProgram m_Program;

	VBOTeapotPatch *m_Teapot;

	mat4 m_Projection;
	mat4 m_View;
	mat4 m_Model;
	mat4 m_Viewport;

	KMouse m_Mouse;
	int m_TessLevel;

	float m_Angle;
};

#endif // TESSELLATION_H