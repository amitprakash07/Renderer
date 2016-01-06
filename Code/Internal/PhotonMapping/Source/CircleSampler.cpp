#include "CircleSampler.h"
#include "utils.h"

CircleSampler::CircleSampler(int i_minSampleCount, int i_maxSampleCount, float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}

void CircleSampler::init(int i_minSampleCount, int i_maxSampleCount, float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}


float CircleSampler::getRadius()
{
	return radius;
}

Point3 CircleSampler::getTargetPosition()
{
	return targetPosition;
}

void CircleSampler::setTargetPosition(Point3 i_target)
{
	targetPosition = i_target;
}

void CircleSampler::setRadius(float i_radius)
{
	radius = i_radius;
}

Point3 CircleSampler::getSampleOrigin()
{
	return sampleOrigin;
}

void CircleSampler::setSampleOrigin(Point3 i_sampleOrigin)
{
	sampleOrigin = i_sampleOrigin;
}


void CircleSampler::generateSamples(float, float)
{
	clearSamples();
	increaseSampleCount();	
	Sample tempSample;
	for (int i = 0; i < getCurrentSampleCount(); i++)
	{
		float sampleRadius = sqrt(getRandomNumber(0,radius));
		float theta = getRandomNumber(0, 360);
		tempSample.setOffset(getCoordinateOnCircle(sampleRadius, theta));
		/*Point3 tempTargetPosition = targetPosition + tempSample.getOffset();
		tempSample.setRay(Ray(sampleOrigin, tempTargetPosition - sampleOrigin));*/
		addSampleToList(tempSample);
	}
}




