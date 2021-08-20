#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include "GLSLProgram.h"
#include "KScene.h"

#include <FreeImage.h>

class SkyBox;
class VBOTeapot;
class VBOTorus;

class EnvironmentMap : public KScene
{
public:
	EnvironmentMap();
	~EnvironmentMap();

	void Init();
	void Render(float dt);
	void Resize(int width, int height);
	void Mouse(const KMouse &mouse);
	void Motion(float x, float y);
	void Keyboard(int key, float x, float y);
	void Idle();
	
private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;
	
	mat4 m_ModelMatrix;
	mat4 m_ViewMatrix;
	mat4 m_ProjectionMatrix;
	
    SkyBox *m_SkyBox;
	VBOTeapot *m_Teapot;
	VBOTorus *m_Torus;

	KMouse m_Mouse;
	KCamera m_Camera;

	KRotater m_Rotater;

	void LoadCubeMap();
	FIBITMAP* GetBitMap(char const * const filename) const;
	void SetMatrices();
};

#endif // ENVIRONMENTMAP_H