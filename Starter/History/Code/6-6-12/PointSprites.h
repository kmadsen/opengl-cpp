#ifndef POINT_SPRITES_H
#define POINT_SPRITES_H

#include "KScene.h"
#include "GLSLProgram.h"

#include <FreeImage.h>

class PointSprites : public KScene
{
public:
	PointSprites(void);
	~PointSprites(void);

	void Init() override;
	void Render() override;
	void Resize(int width, int height) override;
	void Update(float dt) override;
private:
	GLSLProgram m_Program;

	mat4 m_Projection;
	mat4 m_View;
	mat4 m_Model;

	int m_NumSprites;
	float* m_Locations;
    GLuint m_Sprites;

	float m_Angle;
};

#endif POINT_SPRITES_H