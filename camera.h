#pragma once

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <xnamath.h>
#include "maths.h"

class Camera
{
	private:
		float m_x, m_y, m_z, m_dx, m_dy, m_dz, m_camera_rotation, m_pitchDegree,
			m_velocityY, m_velocityYAdd, m_jumpHeigth, m_gravity;
		XMVECTOR m_position, m_lookat, m_up;

	public:
		Camera();
		Camera(float x, float y, float z, float camera_rotation);
		void Rotate(float degrees);
		void Forward(float distance);
		void Up(float distance);
		XMMATRIX GetViewMatrix();
		void Strafe(float distance);
		void Jump();
		void Update();
		void Pitch(float degree);
		float GetX();
		float GetY();
		float GetZ();
		void LookAt(float x, float z);
		void LookAt(float x, float y, float z);
		xyz GetLookDirection();
};