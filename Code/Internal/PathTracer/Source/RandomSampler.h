#ifndef __RANDOM_SAMPLER_H
#define __RANDOM_SAMPLER_H

#include "Sampler.h"


class RandomSampler:public Sampler
{
public:
	void generateSamples(float pixelIndexAlongWidth =0, 
		float pixelIndexAlongHeight =0) override;
			
	RandomSampler(int i_minSampleCount,
		int i_maxSampleCount,
		double i_minVarianceThreshold,
		double i_maxVarianceThreshold);
	void init(int i_minSampleCount,
		int i_maxSampleCount,
		double i_minVarianceThreshold,
		double i_maxVarianceThreshold);
	~RandomSampler(){}
	
};

#endif