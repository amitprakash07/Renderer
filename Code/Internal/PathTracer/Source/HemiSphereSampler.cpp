#include "HemiSphereSampler.h"
#include <time.h>
#include "utils.h"

#define IMPORTANCE_SAMPLING 1

HemiSphereSampler::HemiSphereSampler(int i_minSampleCount, int i_maxSampleCount, float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}

void HemiSphereSampler::init(int i_minSampleCount, int i_maxSampleCount, float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}


float HemiSphereSampler::getRadius()
{
	return radius;
}

Point3 HemiSphereSampler::getTargetPosition()
{
	return targetPosition;
}

void HemiSphereSampler::setTargetPosition(Point3 i_target)
{
	targetPosition = i_target;
}

void HemiSphereSampler::setRadius(float i_radius)
{
	radius = i_radius;
}

Point3 HemiSphereSampler::getSampleOrigin()
{
	return sampleOrigin;
}

void HemiSphereSampler::setSampleOrigin(Point3 i_sampleOrigin)
{
	sampleOrigin = i_sampleOrigin;
}


void HemiSphereSampler::generateSamples(float, float)
{
	clearSamples();
	increaseSampleCount();
	if (IMPORTANCE_SAMPLING == 0)
		generateSampleUnifromly();
	else 
		generateSamplesWithImportanceSampling();
			
}

void HemiSphereSampler::generateSampleUnifromly()
{
	Sample tempSample;
	float tempRadius = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	for (int i = 0; i < getCurrentSampleCount(); ++i)
	{
		tempRadius = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		theta = acos(1 - tempRadius);
		phi = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.0f;
		Point3 offset = getSphericalCoordinates(tempRadius, theta, phi);
		tempSample.setOffset(offset);
		addSampleToList(tempSample);
	}
}

void HemiSphereSampler::generateSamplesWithImportanceSampling()
{
	Sample tempSample;
	Point3 offset;
	float tempRadius = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	float r1 = 0.0f;
	float r2 = 0.0f;;
	for (int i = 0; i < getCurrentSampleCount(); ++i)
	{
		r1 = getRandomNumber(0, 1);
		r2 = getRandomNumber(0, 1);
		phi = 2 * M_PI*r1;
		theta = acosf(sqrtf(r2));
		offset.x = cos(2 * M_PI*r1)* sqrtf(1 - r2);
		offset.y = sin(2 * M_PI*r1)*sqrtf(1 - r2);
		offset.z = sqrtf(r2);
		tempSample.setOffset(offset);
		addSampleToList(tempSample);		
	}
}






