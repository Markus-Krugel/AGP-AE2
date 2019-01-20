#pragma once
#include "objfilemodel.h"

struct Plane
{
	xyz normal; /* the normal to the plane */
	float d; /* the 'd' constant in the equation for this plane */
};


class Maths
{
	public:
		static float dot(xyz* point1, xyz* point2);
		static xyz cross(xyz* point1, xyz* point2);
		static xyz normal(xyz* point1, xyz* point2, xyz* point3);
		static float getLength(xyz* point1, xyz* point2);
		static xyz getRayBetweenTwoPoints(xyz* point1, xyz* point2);
		static xyz vectorToXYZ(XMVECTOR* vector);
		static Plane calculatePlane(xyz* point1, xyz* point2, xyz* point3);
		static float planeEquationForPoint(Plane* plane, xyz* point);
		static xyz planeIntersection(Plane* plane, xyz* point1, xyz* point2);
		static bool in_triangle(xyz* triangle0, xyz* triangle1, xyz* triangle2, xyz* point);
};