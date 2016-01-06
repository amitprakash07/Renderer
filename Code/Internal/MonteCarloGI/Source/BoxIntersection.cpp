#include "scene.h"

#define BOX_BIAS (1200* FLT_EPSILON)
#include <iostream>

bool Box::IntersectRay(const Ray& r, float t_max) const
{
	bool isHit = false;
	float tx1, tx2, txEntry, txExit;
	float ty1, ty2, tyEntry, tyExit;
	float tz1, tz2, tzEntry, tzExit;

	tx1 = tx2 = txEntry = txExit = ty1 = ty2 = tyEntry = tyExit = tz1 = tz2 = tzEntry = tzExit = BIGFLOAT;

	//Check X-Normal
	if (r.dir.x != 0.0f)
	{
		tx1 = (-r.p.x + pmin.x) / r.dir.x;
		tx2 = (-r.p.x + pmax.x) / r.dir.x;

		txEntry = min(tx1, tx2);
		txExit = max(tx1, tx2);
	}
	else
	{
		if (r.p.x > pmax.x || r.p.x < pmin.x)
			return false;
	}


	//Check y-Plane
	if (r.dir.y != 0.0f)
	{
		ty1 = (-r.p.y + pmin.y) / r.dir.y;
		ty2 = (-r.p.y + pmax.y) / r.dir.y;
		tyEntry = min(ty1, ty2);
		tyExit = max(ty1, ty2);
	}
	else
	{
		if (r.p.y > pmax.y || r.p.y < pmin.y)
			return false;
	}

	//Check z-Plane
	if (r.dir.z != 0.0f)
	{
		tz1 = (-r.p.z + pmin.z) / r.dir.z;
		tz2 = (-r.p.z + pmax.z) / r.dir.z;
		tzEntry = min(tz1, tz2);
		tzExit = max(tz1, tz2);
	}
	else
	{
		if (r.p.z > pmax.z || r.p.z < pmin.z)
			return false;
	}
		

	float tEntry = max(txEntry, max(tyEntry, tzEntry));
	float tExit = min(txExit, min(tyExit, tzExit));

	if (tEntry < tExit )
	{
		/*t_max = tEntry;*/
		isHit = true;
		//std::cout << "\nBox InterSection Successful";
	}	
	
	//std::cout << "\nBox InterSection UnSuccessful";
	return isHit;
}


