#include  "objects.h"
#include <iostream>

#define NORMALINTERPOLATE 1
#define SHOW_NORMAL 1
#define TRIANGLEBIAS (2000 * FLT_EPSILON)

typedef cyTriMesh::cyTriFace MeshIndices;

bool TriObj::IntersectRay(const Ray& ray, HitInfo& hInfo, int hitSide) const
{
	//std::cout << NF()<<std::endl;
	bool isHit = false;
//	hInfo.operationCount++;
	if (GetBoundBox().IntersectRay(ray, hInfo.z))
	{
		isHit = TraceBVHNode(ray, hInfo, hitSide, bvh.GetRootNodeID());
		/*for (size_t i = 0; i < NF(); i++)
		{
			isHit |= IntersectTriangle(ray, hInfo, hitSide, i);
		}*/
	}
	return isHit;
}


bool TriObj::IntersectTriangle(const Ray& ray, HitInfo& hInfo, int hitSide, unsigned faceID) const
{
	bool ishit = false;
	/*if (GetBoundBox().IntersectRay(ray, BIGFLOAT))
	{*/
//		hInfo.operationCount++;
		MeshIndices face = this->F(faceID);
		Point3 vertexA = this->V(face.v[0]);
		Point3 vertexB = this->V(face.v[1]);
		Point3 vertexC = this->V(face.v[2]);
		Point3 faceNormalNormalized = Point3(0, 0, 0);
		Point3 normal;
		float t;

		normal = (vertexB - vertexA).Cross(vertexC - vertexA);
		faceNormalNormalized = normal.GetNormalized();
		t = -(ray.p - vertexA).Dot(faceNormalNormalized) / ray.dir.Dot(faceNormalNormalized);

		if ((ray.p - vertexA).Dot(faceNormalNormalized) == 0)
			return false;

		if (t >= TRIANGLEBIAS && t < hInfo.z && hitSide == HIT_FRONT)
		{
			Point3 P = ray.p + t * ray.dir;

			float totalArea = normal.Length() / 2;
			float areaA = (vertexB - P).Cross(vertexC - P).Dot(faceNormalNormalized) / 2;
			float areaB = (vertexC - P).Cross(vertexA - P).Dot(faceNormalNormalized) / 2;
			float areaC = (vertexA - P).Cross(vertexB - P).Dot(faceNormalNormalized) / 2;
			float alpha = areaA / totalArea;
			float beta = areaB / totalArea;
			float gamma = areaC / totalArea;
			if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1)
			{
				if (NORMALINTERPOLATE)
				{
					P = alpha*vertexA + beta*vertexB + gamma*vertexC;
					faceNormalNormalized = GetNormal(faceID, Point3(alpha, beta, gamma));
				}

				hInfo.p = P;
				hInfo.uvw = GetTexCoord(faceID, Point3(alpha, beta, gamma));
#ifdef RELEASE_DEBUG
				if (SHOW_NORMAL)
					hInfo.N = faceNormalNormalized;
				else
					hInfo.N = Point3(beta, gamma, alpha);
#else
				hInfo.N = faceNormalNormalized;
#endif
				hInfo.z = t;

				if (ray.dir.Dot(hInfo.N) > TRIANGLEBIAS)
					hInfo.front = true;
				else  hInfo.front = false;

				ishit = true;
			}
		}

		if (t >= TRIANGLEBIAS && t > hInfo.z && hitSide == HIT_BACK)
		{
			Point3 P = ray.p + t * ray.dir;
			float totalArea = normal.Length() / 2;
			float areaA = (vertexB - P).Cross(vertexC - P).Dot(faceNormalNormalized) / 2;
			float areaB = (vertexC - P).Cross(vertexA - P).Dot(faceNormalNormalized) / 2;
			float areaC = (vertexA - P).Cross(vertexB - P).Dot(faceNormalNormalized) / 2;
			float alpha = areaA / totalArea;
			float beta = areaB / totalArea;
			float gamma = areaC / totalArea;
			if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1)
			{
				if (NORMALINTERPOLATE)
				{
					P = alpha*vertexA + beta*vertexB + gamma*vertexB;
					faceNormalNormalized = GetNormal(faceID, Point3(alpha, beta, gamma));
				}
				hInfo.p = P;
				hInfo.uvw = GetTexCoord(faceID, Point3(alpha, beta, gamma));
#ifdef RELEASE_DEBUG
				if (SHOW_NORMAL)
					hInfo.N = faceNormalNormalized;
				else
					hInfo.N = Point3(beta, gamma, alpha);
#else
				hInfo.N = faceNormalNormalized;
#endif
				if (ray.dir.Dot(hInfo.N) > 0)
					hInfo.front = true;
				else  hInfo.front = false;
				ishit = true;
			}
		}

		if (t < TRIANGLEBIAS)
			ishit = false;
	//}
	
	return ishit;
}

