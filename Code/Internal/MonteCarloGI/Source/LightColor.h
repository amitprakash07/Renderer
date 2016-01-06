#ifndef __LIGHT_COMPONENT_H
#define __LIGHT_COMPONENT_H

#include "scene.h"


inline Color specularComponent(const Light *i_light, Color i_lightColor, Point3 i_viewDirection, HitInfo i_hInfo, Color i_specular, float i_glossiness)
{
	Color o_specularComponent = Color(0, 0, 0);

	float geometryTerm = ((-1)*i_light->Direction(i_hInfo.p)).Dot(i_hInfo.N);
	if (geometryTerm > 0)
	{
		Point3 halfVector = (((-1)*i_light->Direction(i_hInfo.p)) + i_viewDirection).GetNormalized();
		o_specularComponent = i_specular * pow(halfVector.Dot(i_hInfo.N), i_glossiness) *
			/*i_light->Illuminate(i_hInfo.p, i_hInfo.N)*/ i_lightColor *geometryTerm;
	}

	o_specularComponent.ClampMinMax();
	return o_specularComponent;
}

inline Color diffuseComponent(const Light *i_light, Color i_lightColor, HitInfo i_hInfo, Color i_diffuse)
{
	Color o_diffuseComponent = Color(0, 0, 0);
	float geometryTerm = ((-1)*i_light->Direction(i_hInfo.p)).Dot(i_hInfo.N);
	if (geometryTerm > 0)
		o_diffuseComponent = /*i_light->Illuminate(i_hInfo.p, i_hInfo.N)*/i_lightColor * i_diffuse * geometryTerm;

	o_diffuseComponent.ClampMinMax();
	return o_diffuseComponent;
}

inline Color ambientComponent(const Light *i_light, Color i_lightColor, HitInfo i_hInfo, Color i_diffuse)
{
	Color o_ambient = Color(0, 0, 0);
	o_ambient = /*i_light->Illuminate(i_hInfo.p, i_hInfo.N)*/i_lightColor * i_diffuse;
	o_ambient.ClampMinMax();
	return o_ambient;
}

inline Color24 normalColor(HitInfo i_hitInfo)
{
	Point3 tempHit = i_hitInfo.N;
	tempHit *= 255;
	if (tempHit.x < 0)
		tempHit.x *= -1;
	if (tempHit.y < 0)
		tempHit.y *= -1;
	if (tempHit.z < 0)
		tempHit.z *= -1;

	Color24 tempColor;
	tempColor.Set(static_cast<unsigned char> (tempHit.x), static_cast<unsigned char> (tempHit.y), static_cast<unsigned char> (tempHit.z));
	return (tempColor);
}





#endif