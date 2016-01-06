//-------------------------------------------------------------------------------
///
/// \file       lights.h 
/// \author     Cem Yuksel (www.cemyuksel.com)
/// \version    13.0
/// \date       December 1, 2015
///
/// \brief Example source for CS 6620 - University of Utah.
///
//-------------------------------------------------------------------------------

#ifndef _LIGHTS_H_INCLUDED_
#define _LIGHTS_H_INCLUDED_

#include "scene.h"

//-------------------------------------------------------------------------------

class GenLight : public Light
{
protected:
	void SetViewportParam(int lightID, ColorA ambient, ColorA intensity, Point4 pos ) const;
	static float Shadow(Ray ray, float t_max=BIGFLOAT);
};

//-------------------------------------------------------------------------------

class AmbientLight : public GenLight
{
public:
	AmbientLight() : intensity(0,0,0) {}
	virtual Color Illuminate(const Point3 &p, const Point3 &N) const override{ return intensity; }
	virtual Point3 Direction(const Point3 &p) const override{ return Point3(0,0,0); }
	virtual bool IsAmbient() const override{ return true; }
	virtual void SetViewportLight(int lightID) const override{ SetViewportParam(lightID,intensity,0.0f,Point4(0,0,0,1)); }

	void SetIntensity(Color intens) { intensity=intens; }
private:
	Color intensity;
};

//-------------------------------------------------------------------------------

class DirectLight : public GenLight
{
public:
	DirectLight() : intensity(0,0,0), direction(0,0,1) {}
	virtual Color Illuminate(const Point3 &p, const Point3 &N) const override{ return Shadow(Ray(p,-direction)) * intensity; }
	virtual Point3 Direction(const Point3 &p) const override{ return direction; }
	virtual void SetViewportLight(int lightID) const override{ SetViewportParam(lightID,0.0f,intensity,Point4(-direction,0.0f)); }

	void SetIntensity(Color intens) { intensity=intens; }
	void SetDirection(Point3 dir) { direction=dir.GetNormalized(); }
private:
	Color intensity;
	Point3 direction;
};

//-------------------------------------------------------------------------------

class PointLight : public GenLight
{
public:
	PointLight() : intensity(0,0,0), position(0,0,0), size(0) {}
	virtual Color Illuminate(const Point3 &p, const Point3 &N) const override;
	virtual Point3 Direction(const Point3 &p) const override{ return (p-position).GetNormalized(); }
	virtual void SetViewportLight(int lightID) const override;
	void SetIntensity(Color intens) { intensity=intens; }
	void SetPosition(Point3 pos) { position=pos; }
	void SetSize(float s) { size=s; }

	// Photon Extensions
	virtual bool	IsPhotonSource()		const override{ return true; }
	virtual Color	GetPhotonIntensity()	const override{ return intensity; }
	virtual Ray		RandomPhoton()			const override;

private:
	Color intensity;
	Point3 position;
	float size;
};

//-------------------------------------------------------------------------------

#endif
