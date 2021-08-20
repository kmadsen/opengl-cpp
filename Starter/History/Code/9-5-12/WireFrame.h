#ifndef WIRE_FRAME_H
#define WIRE_FRAME_H

#include "KScene.h"
#include "GLSLProgram.h"

class VBOMesh;

class WireFrame : public KScene
{
public:
	WireFrame(void);
	virtual ~WireFrame(void);

	void Init() override;
	void Render() override;
	void Resize(int width, int height) override;
	void Update(float dt) override;
	void Mouse(const KMouse &mouse) override { m_Mouse = mouse; }
	void Motion(float x, float y) override;
private:
	GLSLProgram m_Program;

    VBOMesh *m_Ogre;

	mat4 m_Projection;
	mat4 m_View;
	mat4 m_Model;
	mat4 m_Viewport;

	KMouse m_Mouse;

	float m_Angle;
};

#endif WIRE_FRAME_H
