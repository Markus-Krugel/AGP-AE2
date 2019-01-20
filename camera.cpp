#include "camera.h";

Camera::Camera(float x, float y, float z, float camera_rotation)
{
	m_velocityY = 0;
	m_jumpHeigth = 2;
	m_velocityYAdd = 0.0015f;
	m_gravity = 0.005f;
	m_dy = 0;
	m_pitchDegree = 0;

	m_camera_rotation = camera_rotation;
	m_x = x;
	m_y = y;
	m_z = z;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
}

void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
}

void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

void Camera::Up(float distance)
{
	m_y += distance;
}


XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0);
	m_up = XMVectorSet(0, 1, 0, 0);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

void Camera::Strafe(float distance)
{
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(m_lookat-m_position, m_up));
	XMFLOAT3 direction;

	XMStoreFloat3(&direction, right);

	m_dx = direction.x;
	m_dz = direction.z;

	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

void Camera::Jump()
{
	if (m_velocityY == 0)
		m_velocityY = m_jumpHeigth;
}

void Camera::Update()
{
	if (m_velocityY > 0)
	{
		m_velocityY -= m_velocityYAdd;
		Up(m_velocityY * m_gravity);
	}
	else if (m_velocityY < 0 && m_velocityY > -m_jumpHeigth)
	{
		m_velocityY -= m_velocityYAdd;
		Up(m_velocityY * m_gravity);
	}
	else
		m_velocityY = 0;
}

void Camera::Pitch(float degree)
{
	if (m_pitchDegree + degree < 90 && m_pitchDegree + degree > -90)
	{
		m_pitchDegree += degree;

		m_dy = sin(m_pitchDegree * (XM_PI / 180));
	}
}

float Camera::GetX()
{
	return m_x;
}

float Camera::GetY()
{
	return m_y;
}

float Camera::GetZ()
{
	return m_z;
}

void Camera::LookAt(float x, float z)
{
	float dx = m_x - x;
	float dz = m_z - z;

	m_camera_rotation = atan2(dx, dz) * (180.0 / XM_PI);
}

void Camera::LookAt(float x, float y, float z)
{
	float dx = m_x - x;
	float dy = m_y - y;
	float dz = m_z - z;

	if (dy < 90 && dy > 90)
	{
		m_dy = sin(dy * (XM_PI / 180));

		m_pitchDegree = dy;

		m_camera_rotation = atan2(dx, dz) * (180.0 / XM_PI);
	}
}

xyz Camera::GetLookDirection()
{
	xyz result;
	result.x = m_dx;
	result.y = m_dy;
	result.z = m_dz;

	return result;
	//XMVECTOR lookDirection = XMVectorSet(m_dx, m_dy, m_dz, 0);
	//return lookDirection;
}


