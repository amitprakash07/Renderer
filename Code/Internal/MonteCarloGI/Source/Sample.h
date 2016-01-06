#ifndef __SAMPLE_H
#define __SAMPLE_H

#include "scene.h"
#include <vector>


class Sample
{
public:
	void setRay(Ray);
	Ray getRay();
	void setColor(Color);
	Color getColor();
	void setHitInfo(HitInfo);
	HitInfo getHitInfo();
	void setIshit(bool);
	bool isHitTrue();
	void setOffset(Point3);
	Point3 getOffset();
	Sample();	
private:
	Ray mRay;
	Color mShadeColor;
	HitInfo mHitInfo;
	Point3 offset;
	bool isHit;
};


class SampleList
{

public:
	void addSampleToList(Sample) ;
	int getMaxSampleCount();
	int getMinSampleCount();
	int getCurrentSampleListSize();
	void addColor(int, Color);
	Sample getSample(int);	
	bool isEmpty();
	Color getVariance();
	void clearAll();
	HitInfo getHitInfoForSample(int );
	void setHitInfoForSample(int, HitInfo);
	void setHitForSample(int , bool);
	bool getHitForSample(int );
	Color getMeanColorOfAllSamples();
	float getAverageDepthOfAllHitSamples();
	SampleList(int, int);
	SampleList();
	~SampleList();
private:
	std::vector<Sample> mSamples;
	int minSampleCount;
	int maxSampleCount;
	Color summedShade;

	int getHitSamplesCount();
};

#endif
