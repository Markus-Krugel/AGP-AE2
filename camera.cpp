#include "camera.h";

Camera::Camera()
{
}

// The constructor for the camera class
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

// rotates the camera horizontally
void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
}

// moves forward in look direction
void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

//moves upwards
void Camera::Up(float distance)
{
	m_y += distance;
}

// returns the view matrix of the camera
XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0);
	m_up = XMVectorSet(0, 1, 0, 0);

	XMMATRIX result = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return result;
}

// moves left and right
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

// initiate a jump 
void Camera::Jump()
{
	if (m_velocityY == 0)
		m_velocityY = m_jumpHeigth;
}

// updates the current heigth after a jump
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

// rotates the camera vertically
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

// rotates the camera to look at a certain position
void Camera::LookAt(float x, float z)
{
	float dx = m_x - x;
	float dz = m_z - z;

	m_camera_rotation = atan2(dx, dz) * (180.0 / XM_PI);
}

// rotates the camera to look at a certain position
void Camera::LookAt(float x, float y, float z)
{
	float dx = m_x - x;
	float dy = m_y - y;
	float dz = m_z - z;

	m_dx = dx;
	m_dz = dz;

	m_dy = sin(dy * (XM_PI / 180));
	
	m_pitchDegree = dy;
	
	m_camera_rotation = atan2(dx, dz) * (180.0 / XM_PI);
}

// returns the direction the camera is looking
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


