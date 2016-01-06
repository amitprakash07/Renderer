#ifndef __CIRCLE_SAMPLER_H
#define __CIRCLE_SAMPLER_H

#include "Sampler.h"

class CircleSampler :public Sampler
{
public:
	void generateSamples(float i = 0.0f, float j = 0.0f) override;
	void setRadius(float);
	float getRadius();
	Point3 getSampleOrigin();
	void setSampleOrigin(Point3);
	Point3 getTargetPosition();
	void setTargetPosition(Point3);
	void init(int, int, float, Point3, Point3);
	CircleSampler(int, int, float, Point3, Point3);
private:
	float radius;
	Point3 sampleOrigin;
	Point3 targetPosition;

};

#endif