#ifndef BUMPMAP_H
#define BUMPMAP_H

#include "GLSLProgram.h"
#include "KScene.h"

#include <FreeImage.h>

class VBOMesh;

class BumpMap : public KScene
{
public:
	BumpMap();
	~BumpMap();

	void Init();
	void Render();
	void Resize(int width, int height);
	void Mouse(const KMouse &mouse);
	void Motion(float x, float y);
	void Keyboard(int key, float x, float y);

	void Idle() { }
	
private:
	GLSLProgram m_Program;
	GLuint m_VaoHandle;
    VBOMesh *m_Ogre;
	mat4 m_ProjectionM;

	KMouse m_Mouse;
	KCamera m_Camera;

	KRotater m_Rotater;

	void LoadTextures();
	FIBITMAP* GetBitMap(char const * const filename) const;
};

#endif // BUMPMAP_H