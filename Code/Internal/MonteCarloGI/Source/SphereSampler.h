#ifndef __SPHERE_SAMPLER_H
#define __SPHERE_SAMPLER_H

#include "Sampler.h"

class SphereSampler:public Sampler
{
public :
	void generateSamples(float i = 0.0f, float j = 0.0f) override;
	void setRadius(float);
	float getRadius();
	Point3 getSampleOrigin();
	void setSampleOrigin(Point3);
	Point3 getTargetPosition();
	void setTargetPosition(Point3);
	void init(int, int, float, Point3 i_origin = Point3(0.0f, 0.0f, 0.0f), Point3 i_target = Point3(0.0f, 0.0f, 0.0f));
	SphereSampler(int, int, float, Point3 i_origin = Point3(0.0f,0.0f,0.0f), Point3 i_target = Point3(0.0f,0.0f,0.0f));
private :
	float radius;
	Point3 sampleOrigin;
	Point3 targetPosition;
	
};

#endif