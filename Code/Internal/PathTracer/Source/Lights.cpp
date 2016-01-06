#include "lights.h"
#include "RayIntersection.h"
#include "CircleSampler.h"
#include "utils.h"

extern Node rootNode;

#define SHADOW_SAMPLE_COUNT 8


float GenLight::Shadow(Ray ray, float t_max)
{
	HitInfo hInfo;
	hInfo.Init();
	hInfo.z = t_max;
	if (TraceRay(&rootNode, ray, hInfo, HIT_FRONT))
		return 0.0f;
	return 1.0f;
}


Color PointLight::Illuminate(const Point3& p, const Point3& N) const
{
	Color returnColor = Color(0.0f);
	if (size > 0)
	{
		CircleSampler randomCircleSampler = CircleSampler(SHADOW_SAMPLE_COUNT, SHADOW_SAMPLE_COUNT, size, p, position);
		randomCircleSampler.generateSamples();
		Point3 randomVectorW;
		Point3 vectorU = Point3(0.0f, 0.0f, 0.0f);
		Point3 vectorV = Point3(0.0f, 0.0f, 0.0f);
		Point3 lightDir;
		lightDir = -1 * Direction(p);
		Ray sampleRay;
		Point3 offset;
		Point3 samplePosition;
		getOrthoNormalBasisVector(lightDir, vectorU, vectorV);
		for (int i = 0; i < randomCircleSampler.getCurrentSampleCount(); ++i)
		{
			offset = randomCircleSampler.getSample(i).getOffset();
			samplePosition = position + offset.x*vectorU + offset.y*vectorV;
			sampleRay.dir = samplePosition + lightDir /*Direction(samplePosition)*/;
			sampleRay.p = p;
			randomCircleSampler.setSampleColor(i, Shadow(sampleRay)* intensity);
			randomCircleSampler.setIsSampleHit(i, true);
		}

		returnColor = randomCircleSampler.getAveragedSampleListColor();
	}
	else
		returnColor = Shadow(Ray(p,position-p),1) * intensity;

	//delete randomCircleSampler;
	return returnColor;
}


