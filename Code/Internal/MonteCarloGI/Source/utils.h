#ifndef __UTILS_H
#define __UTILS_H

#include "scene.h"



Point3 getSphericalCoordinates(float i_radius, float i_theta, float i_phi);
Point3 getCoordinateOnCircle(float i_radius, float i_theta);
Point3 getRandomVector();
void getOrthoNormalBasisVector(Point3 i_up, Point3 &o_out_vector /*U*/, Point3& o_vector_right /*v*/);
float getRandomNumber(int i_rangeA, int i_rangeB);
Ray calculatePixelCoords(int pixelPositionAlongWidth,
	int pixelPositonAlongHeight, Point3 positionInsidePixel = Point3(0.5f, 0.5f, 0.5f));


#endif