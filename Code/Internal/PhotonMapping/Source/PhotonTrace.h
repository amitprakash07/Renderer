#ifndef __PHOTON_TRACE_H
#define __PHOTON_TRACE_H

#include "cyPhotonMap.h"
#include "scene.h"
#include "RayIntersection.h"

typedef cyPhotonMap PhotonMap;
#define PHOTON_COUNT 100000
#define PHOTON_BOUNCE_COUNT 5
extern LightList lights;
extern Node rootNode;
extern PhotonMap globalPhotonMap;


bool TracePhoton(Node* i_node, Ray& i_photon, Color &i_power, HitInfo& i_hitInfo, int photonBounceCount);
inline void PhotonTracer()
{
	globalPhotonMap.AllocatePhotons(PHOTON_COUNT);
	int currentPhotonCount = 0;
	for (int i = 0; i < lights.size(); ++i)
	{
		while (currentPhotonCount <= PHOTON_COUNT)
		{
			Color power = lights[i]->GetPhotonIntensity() / PHOTON_COUNT;
			HitInfo hitInfo;
			Ray photonRay = lights[i]->RandomPhoton();
			TracePhoton(&rootNode, photonRay, power, hitInfo, PHOTON_BOUNCE_COUNT);
			currentPhotonCount++;
		}
	}
	
	FILE *fp = fopen("../../../../Assets/photonmap.dat", "wb");
	fwrite(globalPhotonMap.GetPhotons(), sizeof(PhotonMap::Photon), globalPhotonMap.NumPhotons(), fp);
	fclose(fp);
	globalPhotonMap.ScalePhotonPowers(10);
	globalPhotonMap.PrepareForIrradianceEstimation();
}

inline bool TracePhoton(Node* i_node, Ray& i_photon, Color& i_power, HitInfo& i_hitInfo, int photonBounceCount)
{
	if(TraceRay(i_node,i_photon,i_hitInfo))
	{
		Color photonPowerAtSurface = i_power / (i_photon.p - i_hitInfo.p).LengthSquared();
		if (i_hitInfo.node->GetMaterial()->IsPhotonSurface())
		{
			globalPhotonMap.AddPhoton(i_hitInfo.p, i_photon.dir, photonPowerAtSurface);
		}
		if (i_hitInfo.node->GetMaterial()->RandomPhotonBounce(i_photon, photonPowerAtSurface, i_hitInfo)
			&& photonBounceCount > 0)
		{
			TracePhoton(i_node, i_photon, photonPowerAtSurface, i_hitInfo, --photonBounceCount);
		}
		return true;
	}
	return false;
}

#endif
