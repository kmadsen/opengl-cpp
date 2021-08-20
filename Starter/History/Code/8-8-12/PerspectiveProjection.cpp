#include "PerspectiveProjection.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

PerspectiveProjection::PerspectiveProjection(float fov, float ar, float near, float far)
	: m_Perspective(), m_View()
{
	SetPerspective(fov, ar, near, far);
}

PerspectiveProjection::~PerspectiveProjection(void)
{
}

const mat4& PerspectiveProjection::GetPerspectiveMatrix() const
{
	return m_Perspective;
}

void PerspectiveProjection::SetPerspective(float fov, float ar, float near, float far)
{
	m_Perspective = glm::perspective(fov, ar, near, far);
}

const mat4& PerspectiveProjection::GetViewMatrix() const
{
	return m_View;
}

void PerspectiveProjection::SetView(const vec3 &origin, const vec3 &at, const vec3 &up)
{
	m_Origin = origin;
	m_View = glm::lookAt(origin, at, up);
}

const vec3& PerspectiveProjection::GetOrigin() const
{
	return m_Origin;
}

