#include "maths.h"

float Maths::dot(xyz * point1, xyz * point2)
{
	return (point1->x * point2->x) + (point1->y * point2->y) + (point1->z * point2->z);
}

xyz Maths::cross(xyz * point1, xyz * point2)
{
	float x = (point1->y * point2->z) - (point1->z * point2->y);
	float y = (point1->z * point2->x) - (point1->x * point2->z);
	float z = (point1->x * point2->y) - (point1->y * point2->x);

	xyz result;
	result.x = x;
	result.y = y;
	result.z = z;

	return result;
}

xyz Maths::normal(xyz * point1, xyz * point2, xyz * point3)
{
	xyz vector1;
	vector1.x = point1->x - point2->x;
	vector1.y = point1->y - point2->y;
	vector1.z = point1->z - point2->z;

	xyz vector2;
	vector2.x = point1->x - point3->x;
	vector2.y = point1->y - point3->y;
	vector2.z = point1->z - point3->z;

	xyz result = cross(&vector1, &vector2);

	float length = getLength(&vector1, &vector2);

	result.x /= length;
	result.y /= length;
	result.z /= length;

	return result;
}

float Maths::getLength(xyz * point1, xyz * point2)
{
	return sqrt((point1->x * point2->x) + (point1->y * point2->y) + (point1->z * point2->z));
}

xyz Maths::getRayBetweenTwoPoints(xyz * point1, xyz * point2)
{
	xyz ray;
	ray.x = point2->x - point1->x;
	ray.y = point2->y - point1->y;
	ray.z = point2->z - point1->z;

	return ray;
}

xyz Maths::vectorToXYZ(XMVECTOR * vector)
{
	xyz result;
	result.x = vector->x;
	result.y = vector->y;
	result.z = vector->z;

	return result;
}

Plane Maths::calculatePlane(xyz * point1, xyz * point2, xyz * point3)
{
	xyz normalVector = normal(point1, point2, point3);

	float offset = -dot(&normalVector, point1);

	Plane result;
	result.d = offset;
	result.normal = normalVector;

	return result;
}

float Maths::planeEquationForPoint(Plane* plane, xyz * point)
{
	return (plane->normal.x * point->x) + (plane->normal.y * point->y) + (plane->normal.z * point->z) + plane->d;
}

xyz Maths::planeIntersection(Plane * plane, xyz * point1, xyz * point2)
{
	xyz ray = getRayBetweenTwoPoints(point1, point2);

	float t = (plane->d - dot(&plane->normal, point1)) / dot(&plane->normal, &ray);

	xyz result;
	result.x = point1->x + (ray.x * t);
	result.y = point1->y + (ray.y * t);
	result.z = point1->z + (ray.z * t);

	return xyz();
}

bool Maths::in_triangle(xyz * triangle0, xyz * triangle1, xyz * triangle2, xyz * point)
{
	xyz cross1 = cross(&getRayBetweenTwoPoints(triangle0, point), &getRayBetweenTwoPoints(triangle0, triangle1));
	xyz cross2 = cross(&getRayBetweenTwoPoints(triangle1, point), &getRayBetweenTwoPoints(triangle1, triangle2));
	xyz cross3 = cross(&getRayBetweenTwoPoints(triangle2, point), &getRayBetweenTwoPoints(triangle2, triangle0));

	if (dot(&cross1, &cross2) >= 0 && dot(&cross2, &cross3) >= 0 && dot(&cross3, &cross1) >= 0)
		return true;
	else
		return false;
}
