#include "objects.h"

bool TriObj::TraceBVHNode(const Ray& ray, HitInfo& hInfo, int hitSide, unsigned nodeID) const
{
	bool isHit = false;
	if(bvh.IsLeafNode(nodeID))
	{
		if(Box(bvh.GetNodeBounds(nodeID)).IntersectRay(ray,hInfo.z))
		{
			unsigned int noOfFacesInLeafNode = bvh.GetNodeElementCount(nodeID);
			
			for (int i = 0; i < noOfFacesInLeafNode; i++)
			{
				isHit |= IntersectTriangle(ray, hInfo, hitSide, bvh.GetNodeElements(nodeID)[i]);
			}
		}
		else
		{
			isHit = false;
		}
	}
	else
	{
		isHit |= TraceBVHNode(ray, hInfo, hitSide, bvh.GetFirstChildNode(nodeID));
		isHit |= TraceBVHNode(ray, hInfo, hitSide, bvh.GetSecondChildNode(nodeID));
	}
	
	return isHit;
}
