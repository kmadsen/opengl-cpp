#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "kscene.h"
#include "GLSLProgram.h"
#include "PerspectiveProjection.h"

class VBOTeapot;
class VBOTorus;
class VBOPlane;

class ShadowMap : public KScene
{
public:
	ShadowMap(void);
	virtual ~ShadowMap(void);

	void Init() override;
	void Render() override;
	void Resize(int width, int height) override;
	void Mouse(const KMouse &mouse) override { m_Mouse = mouse; }
	void Motion(float x, float y) override;
private:
	void DrawScene();
	void SetMatrices();
	void CreateShadowFBO();
	void SaveDepthBuffer();

private:
	GLsizei m_Width, m_Height;
	double m_Angle;
	
	KMouse m_Mouse;

	GLSLProgram m_Program;
	
	// Shadow map stuff
	GLuint m_ShadowFBO, m_Pass1Index, m_Pass2Index;
	GLsizei m_ShadowMapWidth, m_ShadowMapHeight;
	PerspectiveProjection m_LightFrustum;
	mat4 m_ShadowBias;
	mat4 m_LightPV;


	VBOTeapot *m_Teapot;
	VBOTorus *m_Torus;
	VBOPlane *m_Plane;

	mat4 m_Projection;
	mat4 m_View;
	mat4 m_Model;
	mat4 m_Viewport;
};

#endif // SHADOW_MAP_H