#pragma once
#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "Sample.h"


class Sampler
{
public:
	virtual void generateSamples(float i=0.0f, float j=0.0f) = 0;
	virtual bool needMoreSamples();
	int getSampleBucketSize();
	Ray getSampleRay(int i) ;
	Sample getSample(int i);
	void setSampleColor(int, Color);
	void setIsSampleHit(int, bool) ;
	void setHitInfo(int, HitInfo) ;
	Color getAveragedSampleListColor();
	float getAveragedDepth() ;
	void resetSampler() ;
	void setMinThreshold(double i_minThreshold = 0);
	double getMinThreshold();
	void setMaxThreshold(double i_maxThreshold = 0);
	double getMaxThreshold();
	int getCurrentSampleCount();
	void setCurrentSampleCount(int);
	void clearSamples();
	void increaseSampleCount();
	void addSampleToList(Sample);
	void initSampler(int, int);
	Sampler();
	virtual ~Sampler();
private:
	SampleList mSampleList;
	int currentSampleCount;
	double maxVarianceThreshold;
	double minVarianceThreshold;
	float calculateInterpolatedThreshold();
};

#endif
