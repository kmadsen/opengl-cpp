#ifndef KSCENE_H
#define KSCENE_H

#include <glm/gtc/matrix_transform.hpp>
#include "Constants.h"

class KRotater 
{
public:
	KRotater()
	{
		m_RotateAngles = glm::vec2(0.0f);
	}

	void Rotate(const glm::vec2 &delta)
	{
		m_RotateAngles.x -= static_cast<float>(TO_DEGREES(delta.x));
		m_RotateAngles.y -= static_cast<float>(TO_DEGREES(delta.y));
		if (m_RotateAngles.x > 360.0f)
			m_RotateAngles.x = 0.0f;
		if (m_RotateAngles.x < 0.0f)
			m_RotateAngles.x = 360.0f;
		if (m_RotateAngles.y > 360.0f)
			m_RotateAngles.y = 0.0f;
		if (m_RotateAngles.y < 0.0f)
			m_RotateAngles.y = 360.0f;

		m_Transform = glm::mat4(1.0);
		m_Transform = glm::rotate(m_Transform, m_RotateAngles.y, glm::vec3(1.0f, 0.0f, 0.0f));
		m_Transform = glm::rotate(m_Transform, m_RotateAngles.x, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	const glm::mat4 &GetTransform() { return m_Transform; }
private:
	glm::vec2 m_RotateAngles;
	glm::mat4 m_Transform;
};

struct KCamera
{
	const glm::vec3 GetEye() const { return Eye; }


	void SetEye(float x, float y, float z)
	{
		Eye.x = x; Eye.y = y; Eye.z = z;
		UpdateView();
	}
	
	void SetCenter(float x, float y, float z)
	{
		Center.x = x; Center.y = y; Center.z = z;
		UpdateView();
	}

	void SetUp(float x, float y, float z)
	{
		Up.x = x; Up.y = y; Up.z = z;
		Up = glm::normalize(Up);
		UpdateView();
	}

	glm::mat4 GetView() const
	{
		return View;
	}

	void MoveInViewDirection(float delta)
	{
		glm::vec3 vDir = glm::normalize(Center - Eye) * delta;
		Center += vDir;
		Eye += vDir;
		UpdateView();
	}

	void Rotate(const glm::vec2 &delta)
	{
		Rotater.Rotate(delta);
		UpdateView();
	}

private:
	glm::vec3 Eye;
	glm::vec3 Center;
	glm::vec3 Up;
	glm::mat4 View;
	KRotater Rotater;

	void UpdateView()
	{
		View = glm::lookAt(Eye, Center, Up);
		View = Rotater.GetTransform() * View;
	}
};

struct KMouse
{
	enum Button 
	{
		LEFT   = 0,
		MIDDLE = 1,
		RIGHT  = 2
	};
	enum State
	{
		DOWN    = 0,
		UP      = 1
	};

	inline void SetPosition(float x, float y)
	{
		Position.x = x; Position.y = y;
	}

	Button Button;
	State State;
	glm::vec2 Position;
};

class KScene
{
public:

	virtual void Init() = 0;
	virtual void Render(float dt) = 0;
	virtual void Resize(int width, int height) { };
	virtual void Idle() { }
	virtual void Mouse(const KMouse &mouse) { }
	virtual void Motion(float x, float y) { }
	virtual void Keyboard(int key, float x, float y) { }
};

#endif // KSCENE_H