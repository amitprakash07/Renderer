#include "SphereSampler.h"
#include <time.h>
#include "utils.h"

SphereSampler::SphereSampler(int i_minSampleCount, int i_maxSampleCount, 
	float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}

void SphereSampler::init(int i_minSampleCount, int i_maxSampleCount, 
	float i_radius, Point3 i_origin, Point3 i_target)
{
	sampleOrigin = i_origin;
	initSampler(i_minSampleCount, i_maxSampleCount);
	radius = i_radius;
	targetPosition = i_target;
}


float SphereSampler::getRadius()
{
	return radius;
}

Point3 SphereSampler::getTargetPosition()
{
	return targetPosition;}

void SphereSampler::setTargetPosition(Point3 i_target)
{
	targetPosition = i_target;
}

void SphereSampler::setRadius(float i_radius)
{
	radius = i_radius;
}

Point3 SphereSampler::getSampleOrigin()
{
	return sampleOrigin;
}

void SphereSampler::setSampleOrigin(Point3 i_sampleOrigin)
{
	sampleOrigin = i_sampleOrigin;
}


void SphereSampler::generateSamples(float, float)
{
	clearSamples();
	increaseSampleCount();
	Sample tempSample;
	bool rejectionSampling = false;
	bool sampleFound = false;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float r = 0.0f;
	
	for (int i = 0; i < getCurrentSampleCount(); i++)
	{
		if (rejectionSampling)
		{
			while (true)
			{
				x = 2.0f * getRandomNumber(0,1) - 1.0f;
				y = 2.0f * getRandomNumber(0,1) - 1.0f;
				z = 2.0f * getRandomNumber(0,1) - 1.0f;
				if ((x*x + y*y + z*z) <= 1)
					break;
			}
			x *= radius;
			y *= radius;
			z *= radius;
			/*while (!sampleFound)
			{
				x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				if ((x*x + y*y + z*z) <= (radius *radius))
					sampleFound = true;;
			}
			sampleFound = false;*/
			tempSample.setOffset(Point3(x, y, z));
		}
		
		/*	? = 2?r1
			? = acos(1 – 2r2)
			x = cx + 2Rcos(2?r1) r2(1 – r2)
			y = cy + 2Rsin(2?r1) r2(1 – r2)
			z = cz + R(1 – 2r2)
		*/
		else
		{
			float r1 = getRandomNumber(0, 1);
			float r2 = getRandomNumber(0, 1);
			/*float phi = 2 * M_PI * r1;
			float theta = acos(1 - (2 * r2));*/
			Point3 offset;
			offset.x = 2 * radius * cos(2 * M_PI * r1) * sqrt(r2*(1 - r2));
			offset.y = 2 * radius * sin(2 * M_PI * r1) * sqrt(r2*(1 - r2));
			offset.z = radius * (1 - (2 * r2));
			tempSample.setOffset(offset);
		}
		addSampleToList(tempSample);
	}
}




