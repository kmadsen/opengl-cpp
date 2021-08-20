#ifndef PROJECTIVE_TEXTURE_H
#define PROJECTIVE_TEXTURE_H

#include "GLSLProgram.h"
#include "KScene.h"

#include <FreeImage.h>

class VBOPlane;
class VBOTeapot;

class ProjectiveTexture : public KScene
{
public:
	ProjectiveTexture();
	~ProjectiveTexture();

	void Init();
	void Render();
	void Resize(int width, int height);
	void Mouse(const KMouse &mouse);
	void Motion(float x, float y);
	void Keyboard(int key, float x, float y);
	void Idle();
	
private:
	GLSLProgram m_Program;

	mat4 m_ModelMatrix;
	mat4 m_ViewMatrix;
	mat4 m_ProjectionMatrix;

	KMouse m_Mouse;
	KCamera m_Camera;

	VBOPlane *m_Plane;
	VBOTeapot *m_Teapot;
	KRotater m_Rotater;

	void LoadTextures();
	void SetMatrices();
};

#endif // PROJECTIVE_TEXTURE_H