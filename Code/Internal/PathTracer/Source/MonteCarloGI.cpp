#include "MonteCarloGI.h"
#include "HemiSphereSampler.h"
#include "RayIntersection.h"



Color MonteCarloGI::indirectIlluminate(HitInfo &i_hitInfo,LightList& lights, int bounceCount, int i_GISampleCount)
{
	Color indirectIlluminateColor = Color(0.0f);
	Sampler * hemiSphereSampler = new HemiSphereSampler(i_GISampleCount, i_GISampleCount, 1);
	hemiSphereSampler->generateSamples();
	Ray giRay;
	for (int i = 0; i < hemiSphereSampler->getCurrentSampleCount();++i)
	{
		giRay.dir = hemiSphereSampler->getSample(i).getOffset() - i_hitInfo.p;
		giRay.p = i_hitInfo.p;
		if(TraceRay(const_cast<Node*>(i_hitInfo.node),giRay,i_hitInfo))
		{
			indirectIlluminateColor +=i_hitInfo.node->GetMaterial()->Shade(giRay, i_hitInfo, lights, 3);
			indirectIlluminateColor += Color(1.0f, 1.0f, 1.0f);
		}
	}
	delete hemiSphereSampler;
	if(bounceCount>0)
	{
		--bounceCount;
		indirectIlluminateColor += indirectIlluminate(i_hitInfo, lights, bounceCount, 5);
	}

	indirectIlluminateColor = (2 * M_PI) / i_GISampleCount * indirectIlluminateColor;
	return indirectIlluminateColor;
}

MonteCarloGI::MonteCarloGI()
{
	
}

MonteCarloGI::~MonteCarloGI()
{
	
}


