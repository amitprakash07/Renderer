#include "RandomSampler.h"
#include  "utils.h"
#include "Sampler.h"

RandomSampler::RandomSampler(int i_minSampleCount, int i_maxSampleCount, double i_minVarianceThreshold, double i_maxVarianceThreshold)
{
	setMinThreshold(i_minVarianceThreshold);
	setMaxThreshold(i_maxVarianceThreshold);
	initSampler(i_minSampleCount, i_maxSampleCount);
	setCurrentSampleCount(0);
}

void RandomSampler::init(int i_minSampleCount, int i_maxSampleCount, double i_minVarianceThreshold, double i_maxVarianceThreshold)
{
	setMinThreshold(i_minVarianceThreshold);
	setMaxThreshold(i_maxVarianceThreshold);
	initSampler(i_minSampleCount, i_maxSampleCount);
	setCurrentSampleCount(0);
}


void RandomSampler::generateSamples(float pixelIndexAlongWidth, float pixelIndexAlongHeight)
{
	clearSamples();
	increaseSampleCount();
	Point3 tempOffset;
	Sample tempSample;
	for (int i = 0; i < getCurrentSampleCount(); ++i)
	{
		tempOffset.x = 2 * getRandomNumber(0, 1) - 1;
		tempOffset.y = 2 * getRandomNumber(0, 1) - 1;
		tempOffset.z = 2 * getRandomNumber(0, 1) - 1;
		tempSample.setRay(calculatePixelCoords(pixelIndexAlongWidth, pixelIndexAlongHeight, tempOffset));
		tempSample.setColor(Color(0.0f));
		addSampleToList(tempSample);
	}	
}






















