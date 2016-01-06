#define _CRT_RAND_S
#include <stdlib.h>
#include <limits.h>
#include "CircleSampler.h"
#include "utils.h"
#define RANDOMCOSINEANGLE 0.75

extern Camera camera;

float getRandomNumber(int i_rangeA, int i_rangeB)
{
	unsigned int number;
	double randomNumber = 0.0f;
	errno_t err;
	err = rand_s(&number);
	if(err == 0 )
	{
		randomNumber = (static_cast<double>(number) / static_cast<double>(UINT_MAX) * (static_cast<double>(i_rangeB))) ;
		if (randomNumber < static_cast<double>(i_rangeA))
			randomNumber += i_rangeA;
	}
	return randomNumber;
}

void getOrthoNormalBasisVector(Point3 i_up, Point3 &o_out_vector /*U*/, Point3& o_vector_right /*v*/)
{
	Point3 randomVectorW;
	//bool foundRandomVector = false;
	while (true)
	{
		randomVectorW = getRandomVector();
		if ( fabs(i_up.Dot(randomVectorW)) < RANDOMCOSINEANGLE)
		{
			o_out_vector = i_up.Cross(randomVectorW);
			o_vector_right = i_up.Cross(o_out_vector).GetNormalized();
			o_out_vector.Normalize();
			break;
		}
	}
}

Point3 getRandomVector()
{
	Point3 randomVector;
	randomVector.x = 2.0f * getRandomNumber(0, 1) - 1.0f;
	randomVector.y = 2.0f * getRandomNumber(0, 1) - 1.0f;
	randomVector.z = 2.0f * getRandomNumber(0, 1) - 1.0f;
	return randomVector;
}

Point3 getCoordinateOnCircle(float i_radius, float i_theta)
{
	i_theta = i_theta * (M_PI / 180);
	Point3 circularCoordinate;
	circularCoordinate.x = i_radius * cos(i_theta);
	circularCoordinate.y = i_radius * sin(i_theta);
	circularCoordinate.z = 0.0f;
	return circularCoordinate;
}

Point3 getSphericalCoordinates(float i_radius, float i_theta, float i_phi)
{
	i_theta = i_theta * (M_PI / 180);
	i_phi = i_phi * (M_PI / 180);
	Point3 sphericalCoordinate;
	sphericalCoordinate.x = i_radius * sin(i_theta) * sin(i_phi);
	sphericalCoordinate.y = i_radius * cos(i_theta);
	sphericalCoordinate.z = i_radius * sin(i_theta) * cos(i_phi);
	return sphericalCoordinate;
}


Ray calculatePixelCoords(int pixelPositionAlongWidth,
	int pixelPositonAlongHeight, Point3 positionInsidePixel)
{
	Point3 cameraRight = (camera.dir ^ camera.up).GetNormalized();
	double aspectRatio = static_cast<double>(camera.imgWidth) /
		static_cast<double>(camera.imgHeight);

	float camera_l;
	float imagePlaneHeight = 1.0f;

	if (camera.focaldist > 1.0f)
	{
		camera_l = camera.focaldist;
		imagePlaneHeight = camera_l * tan((camera.fov / 2) * (M_PI / 180));
	}
	else
	{
		camera_l = 1 / (tan((camera.fov / 2) * (M_PI / 180)));
	}

	Point3 Sx = cameraRight;
	Point3 Sy = (-1.0f) * camera.up;
	Point3 pixel;
	Point3 k = camera.pos + camera_l* camera.dir;// -cameraRight + camera.up;
	float flipped_i = camera.imgHeight - pixelPositonAlongHeight - 1;
	pixel = k +
		(imagePlaneHeight * aspectRatio * ((2 * ((pixelPositionAlongWidth + positionInsidePixel.x) / camera.imgWidth)) - 1))*Sx +
		(imagePlaneHeight * ((2 * ((flipped_i + positionInsidePixel.y) / camera.imgHeight)) - 1))*Sy;

	Ray sampleRay;
	Sample cameraSample;

	if (camera.dof > 0.0f)
	{
		CircleSampler circleRandomSampler = CircleSampler(2, 2, camera.dof, camera.pos, camera.pos);
		circleRandomSampler.generateSamples();
		int sampleCount = circleRandomSampler.getCurrentSampleCount();
		cameraSample = circleRandomSampler.getSample(static_cast<int>(getRandomNumber(0,sampleCount)));
		Point3 offset = cameraSample.getOffset();
		sampleRay.p = camera.pos + offset;
		sampleRay.dir = (pixel - sampleRay.p).GetNormalized();
		//delete circleRandomSampler;
	}
	else
	{
		sampleRay.p = camera.pos;
		sampleRay.dir = (pixel - camera.pos).GetNormalized();
	}
	return sampleRay;
}