#include "objects.h"

#define BIAS ( 2000* FLT_EPSILON)

bool Sphere::IntersectRay(const Ray& ray, HitInfo& hInfo, int hitSide) const
{
	bool isHit = false;
	//hInfo.operationCount++;
	/*if (GetBoundBox().IntersectRay(ray, BIGFLOAT))
	{*/
		
		float a = ray.dir.Dot(ray.dir);
		float b = 2 * ray.p.Dot(ray.dir);
		float c = ray.p.Dot(ray.p) - 1;

		float d = (b*b) - (4.0f * a * c);

		if (d >= 0.0f)
		{
			float t1 = ((-b) + sqrt(d)) / (2.0f * a);
			float t2 = ((-b) - sqrt(d)) / (2.0f * a);

			/*if (t2 < BIAS && t1 < BIAS)
				isHit = false;

			if (t2 < BIAS && t1 > BIAS)
				hInfo.front = false;

			if (t2 < BIAS && t1 > BIAS)
				hInfo.front = false;*/

			if (t1 < BIAS || t2 < BIAS)
				isHit = false;
			else
			{
				float t;
				if (t1<t2)
					t = t1;
				else
					t = t2;
				if (t<hInfo.z)
				{
					isHit = true;
					hInfo.z = t2;
					hInfo.p = ray.p + hInfo.z* ray.dir;
					hInfo.N = (hInfo.p - Point3(0.0f, 0.0f, 0.0f)).GetNormalized();
					hInfo.front = true;

					float theta = acos(hInfo.p.z);
					float phi = asin(hInfo.p.x / sin(theta));
					hInfo.uvw.x = static_cast<float>(phi / (2 * M_PI));
					hInfo.uvw.y = static_cast<float>(1 - (theta / M_PI));
				}
			}


			/*if ((t2 > BIAS) && (t2 < hInfo.z))
			{
				isHit = true;
				hInfo.z = t2;
				hInfo.p = ray.p + hInfo.z* ray.dir;
				hInfo.N = (hInfo.p - Point3(0.0f, 0.0f, 0.0f)).GetNormalized();
				hInfo.front = true;

				float theta = acos(hInfo.p.z);
				float phi = asin(hInfo.p.x / sin(theta));
				hInfo.uvw.x = static_cast<float>(phi / (2 * M_PI));
				hInfo.uvw.y = static_cast<float>(1 - (theta / M_PI));
			}

			if ((t1 > BIAS) && (t1 < t2))
			{
				isHit = true;
				hInfo.z = t1;
				hInfo.p = ray.p + hInfo.z* ray.dir;
				hInfo.N = (hInfo.p - Point3(0.0f, 0.0f, 0.0f)).GetNormalized();
				hInfo.front = false;

				float theta = acos(hInfo.p.z);
				float phi = asin(hInfo.p.x / sin(theta));
				hInfo.uvw.x = static_cast<float>(phi / (2 * M_PI));
				hInfo.uvw.y = static_cast<float>(1 - (theta / M_PI));
			}*/

		}
		else
		{
			isHit = false;
		}
	/*}
	else
		isHit = false;*/

	return isHit;
}