#include "Sample.h"


void SampleList::addSampleToList(Sample i_sample)
{
	mSamples.push_back(i_sample);
}

int SampleList::getMaxSampleCount()
{
	return maxSampleCount;
}

int SampleList::getCurrentSampleListSize()
{
	return mSamples.size();
}

void SampleList::addColor(int i, Color i_color)
{
	mSamples[i].setColor(i_color);
	summedShade += i_color;
}

Sample SampleList::getSample(int i_index)
{
	if(i_index< mSamples.size())
		return mSamples[i_index];
	Sample dummy = Sample();
	return dummy;
}

Color SampleList::getVariance()
{
	int currentSampleCount = getHitSamplesCount();
	Color mean = summedShade / currentSampleCount ;
	Color squaredDifference = Color(0.0f);

	for (int i = 0; i < mSamples.size(); ++i)
	{
		Color tempDiff = mSamples[i].getColor() - mean;
		squaredDifference += Color(tempDiff.r*tempDiff.r,
			tempDiff.g* tempDiff.g,
			tempDiff.b* tempDiff.b);
	}
	return (squaredDifference / currentSampleCount);
}

SampleList::SampleList(int i_minSampleCount, int i_maxSampleCount)
{
	minSampleCount = i_minSampleCount;
	maxSampleCount = i_maxSampleCount;
	mSamples.reserve(maxSampleCount);
	summedShade = Color(0.0f);
}

SampleList::~SampleList()
{
	mSamples.clear();
}

int SampleList::getMinSampleCount()
{
	return minSampleCount;
}

HitInfo SampleList::getHitInfoForSample(int i_index)
{
	return mSamples[i_index].getHitInfo();
}

void SampleList::setHitInfoForSample(int i_index, HitInfo i_hitInfo)
{
	mSamples[i_index].setHitInfo(i_hitInfo);
}

void SampleList::setHitForSample(int i_index, bool status)
{
	mSamples[i_index].setIshit(status);
}

bool SampleList::getHitForSample(int i_index)
{
	return mSamples[i_index].isHitTrue();
}

float SampleList::getAverageDepthOfAllHitSamples()
{
	float averageDepth = 0.0f;
	for (int i = 0; i < mSamples.size(); ++i)
	{
		if (mSamples[i].isHitTrue())
			averageDepth += mSamples[i].getHitInfo().z;
	}
	if (averageDepth != 0.0f)
		averageDepth /= getHitSamplesCount();
	else
		averageDepth = BIGFLOAT;
	return averageDepth;
}

bool SampleList::isEmpty()
{
	if (mSamples.size() == 0)
		return false;
	return true;
}

SampleList::SampleList()
{
	minSampleCount = 0;
	maxSampleCount = 0;
	summedShade = Color(0.0f);
}

Color SampleList::getMeanColorOfAllSamples()
{	
	int hitCount = getHitSamplesCount();
	if (hitCount > 0)
		summedShade /= hitCount;
	else
		summedShade = Color(0.0f);
	return (summedShade);
}

void SampleList::clearAll()
{
	mSamples.clear();
	summedShade = Color(0.0f);
}

int SampleList::getHitSamplesCount()
{
	int count = 0;
	for (int i = 0; i < mSamples.size(); ++i)
	{
		if (mSamples[i].isHitTrue())
			count++;
	}
	return count;
}

Color Sample::getColor()
{
	return mShadeColor;
}

void Sample::setColor(Color i_color)
{
	mShadeColor = i_color;
}

Ray Sample::getRay()
{
	return mRay;
}

void Sample::setRay(Ray i_ray)
{
	mRay = i_ray;
}

HitInfo Sample::getHitInfo()
{
	return mHitInfo;
}

void Sample::setHitInfo(HitInfo i_hitInfo)
{
	mHitInfo = i_hitInfo;
}

bool Sample::isHitTrue()
{
	return isHit;
}

void Sample::setIshit(bool i_isHit)
{
	isHit = i_isHit;
}

Sample::Sample()
{
	mRay.p = Point3(0.0f, 0.0f, 0.0f);
	mRay.dir = Point3(0.0f, 0.0f, 0.0f);
	mShadeColor = Color(0.0f);
	offset = Point3(0.0f, 0.0f, 0.0f);
	mHitInfo.Init();
	isHit = false;
}

Point3 Sample::getOffset()
{
	return offset;
}

void Sample::setOffset(Point3 i_offset)
{
	offset = i_offset;
}
















