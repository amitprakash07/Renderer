#include "Sampler.h"

Sampler::Sampler()
{
	minVarianceThreshold = 0.0;
	maxVarianceThreshold = 0.0;
	currentSampleCount = 0;
}

void Sampler::initSampler(int i_minCount, int i_maxCount)
{
	mSampleList = SampleList(i_minCount, i_maxCount);
}

int Sampler::getSampleBucketSize()
{
	return currentSampleCount;
}

Sample Sampler::getSample(int i)
{
	return(mSampleList.getSample(i));
}


Ray Sampler::getSampleRay(int i)
{
	return(mSampleList.getSample(i).getRay());
}

void Sampler::setHitInfo(int i_index, HitInfo i_hnfo)
{
	mSampleList.setHitInfoForSample(i_index, i_hnfo);
}

void Sampler::setIsSampleHit(int i_index, bool isHit)
{
	mSampleList.setHitForSample(i_index, isHit);
}

Color Sampler::getAveragedSampleListColor()
{
	Color averageColor = mSampleList.getMeanColorOfAllSamples();
	return averageColor;
}

float Sampler::getAveragedDepth()
{
	return mSampleList.getAverageDepthOfAllHitSamples();
}


void Sampler::setSampleColor(int i_index, Color i_color)
{
	mSampleList.addColor(i_index, i_color);
}

bool Sampler::needMoreSamples()
{
	if (currentSampleCount == 0)
		return true;

	if (currentSampleCount >= mSampleList.getMaxSampleCount())
		return false;

	double interpolatedVarianceThreshold = calculateInterpolatedThreshold();

	Color varianceColor = mSampleList.getVariance();
	if (varianceColor.r > interpolatedVarianceThreshold
		&& varianceColor.g > interpolatedVarianceThreshold
		&& varianceColor.b > interpolatedVarianceThreshold)
		return true;

	return false;

}

void Sampler::increaseSampleCount()
{
	if (currentSampleCount == 0)
		currentSampleCount = mSampleList.getMinSampleCount();
	else
		currentSampleCount *= 2;
}

float Sampler::calculateInterpolatedThreshold()
{
	int maxSampleCount = mSampleList.getMaxSampleCount();
	int minSampleCount = mSampleList.getMinSampleCount();
	float threshold = minVarianceThreshold +
		(maxVarianceThreshold - minVarianceThreshold)*
		((maxSampleCount - currentSampleCount) / (maxSampleCount - minSampleCount));
	return threshold;
}

void Sampler::resetSampler()
{
	mSampleList.clearAll();
	currentSampleCount = 0;
}

int Sampler::getCurrentSampleCount()
{
	return currentSampleCount;
}

void Sampler::addSampleToList(Sample i_sample)
{
	mSampleList.addSampleToList(i_sample);
}

void Sampler::clearSamples()
{
	mSampleList.clearAll();
}

double Sampler::getMaxThreshold()
{
	return maxVarianceThreshold;
}

double Sampler::getMinThreshold()
{
	return minVarianceThreshold;
}


void Sampler::setCurrentSampleCount(int i_count)
{
	currentSampleCount = i_count;
}

void Sampler::setMaxThreshold(double i_maxThreshold)
{
	maxVarianceThreshold = i_maxThreshold;
}

void Sampler::setMinThreshold(double i_minThreshold)
{
	minVarianceThreshold = i_minThreshold;
}


Sampler::~Sampler()
{
	
}







