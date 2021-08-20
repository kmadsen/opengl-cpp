#ifndef ANIMATING_SURFACE_H
#define ANIMATING_SURFACE_H

#include "KScene.h"
#include "GLSLProgram.h"

class VBOPlane;
class PerspectiveProjection;

class AnimatingSurface : public KScene
{
public:
	AnimatingSurface(void);
	virtual ~AnimatingSurface(void);

	void Init() override;
	void Render() override;
	void Resize(int width, int height) override;
	void Mouse(const KMouse &mouse) override { m_Mouse = mouse; }
	void Motion(float x, float y) override;
	void Update(float dt) override;
private:
	GLsizei m_Width, m_Height;
	double m_Angle;
	
	// Wave info
	float m_Time;
	float m_WaveLength;
	float m_Amplitude;
	float m_Velocity;

	KMouse m_Mouse;

	GLSLProgram m_Program;
	
	// Shadow map stuff
	PerspectiveProjection* m_CameraFrustum;
	mat4 m_Model;

	VBOPlane *m_Plane;
};

#endif // ANIMATING_SURFACE_H

