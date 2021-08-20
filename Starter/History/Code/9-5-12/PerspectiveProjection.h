#ifndef PERSPECTIVE_PROJECTION_H
#define PERSPECTIVE_PROJECTION_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;

class PerspectiveProjection
{
public:
	PerspectiveProjection(float fov = 50.0f, float ar = 1.0f, float near = 1.0f, float far = 256.0f);
	virtual ~PerspectiveProjection(void);

	const mat4& GetPerspectiveMatrix() const;
	void SetPerspective(float fov, float ar, float near, float far);
	
	const mat4& GetViewMatrix() const;
	void SetView(const vec3 &origin, const vec3 &at, const vec3 &up);

	const vec3& GetOrigin() const; 
private:
	mat4 m_Perspective;
	mat4 m_View;
	vec3 m_Origin;
};

#endif // PERSPECTIVE_PROJECTION_H

