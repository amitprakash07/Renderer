#ifndef __PHOTON_TRACE_H
#define __PHOTON_TRACE_H

#include "cyPhotonMap.h"
#include "scene.h"
#include "RayIntersection.h"

typedef cyPhotonMap PhotonMap;
#define PHOTON_COUNT 100000
#define PHOTON_BOUNCE_COUNT 10
extern LightList lights;
extern Node rootNode;
extern PhotonMap globalPhotonMap;
extern PhotonMap causticPhotonMap;


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
	FILE *causticfp = fopen("../../../../Assets/causticMap.dat", "wb");
	fwrite(globalPhotonMap.GetPhotons(), sizeof(PhotonMap::Photon), causticPhotonMap.NumPhotons(), causticfp);
	fclose(causticfp);

	globalPhotonMap.ScalePhotonPowers(5);
	globalPhotonMap.PrepareForIrradianceEstimation();
}

inline bool TracePhoton(Node* i_node, Ray& i_photon, Color& i_power, HitInfo& i_hitInfo, int photonBounceCount)
{
	if(TraceRay(i_node,i_photon,i_hitInfo))
	{
		Color photonPowerAtSurface = i_power /*/ (i_photon.p - i_hitInfo.p).LengthSquared()*/;
		//photonPowerAtSurface *= i_hitInfo.N.Dot(-i_photon.dir);
		if (i_hitInfo.node->GetMaterial()->IsPhotonSurface())
		{
			globalPhotonMap.AddPhoton(i_hitInfo.p, i_photon.dir, photonPowerAtSurface);
			if (i_hitInfo.node->GetMaterial()->IsSpecular() && photonBounceCount < PHOTON_BOUNCE_COUNT)
				causticPhotonMap.AddPhoton(i_hitInfo.p, i_photon.dir, photonPowerAtSurface);
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
