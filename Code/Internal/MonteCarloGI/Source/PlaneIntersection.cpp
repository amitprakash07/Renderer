#include "objects.h"

#define PLANEBIAS ( 2000* FLT_EPSILON)

bool Plane::IntersectRay(const Ray& ray, HitInfo& hInfo, int hitSide) const
{
	/* 
	Plane Equation N.X + D = 0, N is the normal vector, X is the position on the plane and D is the translation from (0,0,0)
	We are in plane coordinate, so normal will be always in Z direction
	As Normal is in Z direction, we have to only calculate z component 
	and we are in plane coordinate thus D(of the plane equation)=0
	*/
	//hInfo.operationCount++;
	float t = 0.0f;
	t = -(ray.p.z / ray.dir.z);
	bool isHit = false;
	if(t >= PLANEBIAS && t < hInfo.z && hitSide == HIT_FRONT)
	{
		hInfo.p = ray.p + t * ray.dir;
		hInfo.uvw = (hInfo.p + Point3(1.0f,1.0f,1.0f))/2;
		hInfo.N = Point3(0, 0, 1);
		hInfo.z = t;
		if (ray.dir.Dot(hInfo.N) > 0)
			hInfo.front = false;
		else hInfo.front = true;
		isHit = true;
	}

	if(t >= PLANEBIAS && t> hInfo.z && hitSide==HIT_BACK)
	{
		hInfo.p = ray.p + t * ray.dir;
		hInfo.uvw = (hInfo.p  + Point3(1.0f, 1.0f, 1.0f))/2;
		hInfo.N = Point3(0, 0, 1);
		hInfo.z = t;
		if (ray.dir.Dot(hInfo.N) > 0)
			hInfo.front = false;
		else hInfo.front = true;
		isHit = true;
	}

	if(t < PLANEBIAS && t< hInfo.z)
	{
		isHit = false;
	}

	return isHit;
}
