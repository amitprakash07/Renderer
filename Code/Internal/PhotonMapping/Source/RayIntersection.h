#ifndef __RAY_CASTING_H
#define __RAY_CASTING_H

#include  "objects.h"
#include <limits>

//extern Camera camera;
//extern Ray pixelRay;

inline bool TraceRay(Node * i_node, Ray &ray, HitInfo &hInfo, int hitside = HIT_FRONT)
{
	bool isHit = false;
	Ray transformedRay = i_node->ToNodeCoords(ray);
	
	if (i_node->GetNodeObj() != nullptr/* && i_node->GetNodeObj()->GetBoundBox().IntersectRay(transformedRay,hInfo.z)*/)
	{
		isHit = i_node->GetNodeObj()->IntersectRay(transformedRay, hInfo, hitside);
		if (isHit)
		{
			hInfo.node = i_node;
			hInfo.node->FromNodeCoords(hInfo);
		}
	}
	if (i_node->GetNumChild() > 0)
	{
		for (int i = 0; i < i_node->GetNumChild(); i++)
		{
			Node * childNode = i_node->GetChild(i);
			/*if (i_node->GetChildBoundBox().IntersectRay(ray, BIGFLOAT))
			{*/
				bool tempHit = TraceRay(childNode, transformedRay, hInfo, hitside);
				if (tempHit)
				{
					i_node->FromNodeCoords(hInfo);
				}
				isHit |= tempHit;
			/*}
			else
			{
				isHit = false;
			}*/
		}
	}
	
	return isHit;
}

#endif