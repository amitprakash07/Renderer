#include "materials.h"
#include "RayIntersection.h"
#include "LightColor.h"
#include "SphereSampler.h"
#include "HemiSphereSampler.h"
#include "utils.h"
#include "cyPhotonMap.h"
#include <algorithm>
#include <vector>


#define MONTECARLO_GI 1
#define PATH_TRACING 0
#define GI_ALGO MONTECARLO_GI
#include <map>

extern Node rootNode;
extern TexturedColor environment;
extern int __gi_sampleCount;
typedef cyPhotonMap PhotonMap;
extern PhotonMap globalPhotonMap;
Point3 getRefractionVector(Point3 view, Point3 normal, float n1 = 1, float n2 = 1);
Point3 getReflectionVector(Point3 view, Point3 normal);
Point3 getPerturbedNormal(Point3 normal, Point3 hitPoint, float i_glossiness);
Point3 getRandomDirectionOnHemiSphere(Point3 normal, Point3 hitPoint, float i_glossiness);


float addColorComp(Color i_color);
float averageColorComp(Color i_color);
float maxColorComp(Color i_color);

enum BOUNCE_TYPE
{
	DIFFUSE_REFLECTION = 0,
	SPECULAR_REFLECTION = 1,
	ABSORPTION = 2,
	REFRACTION = 3,
	UNKNOWN = 4
};

void sortLists(float*,BOUNCE_TYPE*, int);

Color MtlBlinn::Shade(const Ray &ray, const HitInfo &hInfo, const LightList &lights, int reflection_bounceCount, int gi_bounceCount) const
{
	Color ambientComp, diffuseComp, specularComp, reflectiveComp, refractiveComp, reflectionTotal, refractionTotal, noColor, diffuseReflection;
	diffuseReflection = ambientComp = diffuseComp = specularComp = reflectiveComp = refractiveComp  = noColor = reflectionTotal = refractionTotal = Color(0.0f, 0.0f, 0.0f);
	Color fromReflection = Color(0.0f, 0.0f, 0.0f);
	Color fromRefraction = Color(0.0f, 0.0f, 0.0f);
	Point3 viewDirection = -ray.dir;
	float schlicksConstant, ratioOfRefraction;
	Color kd = diffuse.Sample(hInfo.uvw);

	int hitside = HIT_FRONT;
	float n1 = 1;
	float n2 = ior;

	for (int i = 0; i < lights.size(); i++)
	{
		Color lightColor = lights[i]->Illuminate(hInfo.p, hInfo.N);
		/*if (lightColor != noColor)
		{*/
			if (lights[i]->IsAmbient())
			{
				ambientComp = ambientComponent(lights[i], lightColor, hInfo, diffuse.Sample(hInfo.uvw));
			}
			else
			{
				//lightColor.ClampMinMax();
				Point3 rayDir = ray.dir;
				globalPhotonMap.EstimateIrradiance<50>(lightColor, rayDir, 2.0f, hInfo.p, &hInfo.N);
				diffuseComp = diffuseComponent(lights[i], lightColor, hInfo, diffuse.Sample(hInfo.uvw));
				specularComp = specularComponent(lights[i], lightColor, viewDirection, hInfo, specular.Sample(hInfo.uvw), glossiness);
			}
		//}
	}
			/************************Refraction************************************************************/
	if(refraction.Sample(hInfo.uvw) != noColor && reflection_bounceCount > 0)
	{
		Ray refractionRay;
		HitInfo refractionRayHit;
		refractionRayHit.Init();
		refractionRay.p = hInfo.p;
		if (hInfo.front == HIT_FRONT)
		{
			refractionRay.dir = getRefractionVector(viewDirection, getPerturbedNormal(hInfo.N, hInfo.p, refractionGlossiness), n1, n2);
		}
		else
		{
			refractionRay.dir = getRefractionVector(viewDirection, getPerturbedNormal( -hInfo.N, hInfo.p, refractionGlossiness), n2, n1);
		}
				
		if(TraceRay(&rootNode, refractionRay, refractionRayHit, hitside))
		{
			Point3 refractionDir = refractionRay.dir;
			refractiveComp += refractionRayHit.node->GetMaterial()->Shade(refractionRay, refractionRayHit, 
				lights, --reflection_bounceCount);
			refractiveComp *= refraction.Sample(hInfo.uvw);
		}
		else
		{
			refractiveComp =  environment.SampleEnvironment(refractionRay.dir);
		}				
	}


	/********************Schlick's Approximation - Fresnel Reflection***************************/
	schlicksConstant = pow(((n1 - n2) / (n1 + n2)), 2);
	ratioOfRefraction = schlicksConstant + (1 - schlicksConstant) * pow((1 - viewDirection.Dot(hInfo.N)), 5);
	reflectionTotal = ratioOfRefraction*refraction.Sample(hInfo.uvw);
	refractionTotal = (1 - ratioOfRefraction)*refraction.Sample(hInfo.uvw);
	///*******************************************************************************************/
	//refractiv    eComp *= refractionTotal; //It = (1-R) * KT'
	reflectionTotal += reflection.Sample(hInfo.uvw); //Doing outside in case refraction didn't occured at all
	/*********************************************************************************************/

	/**********************Reflection*************************************************************/
	if(reflectionTotal != noColor && reflection_bounceCount > 0)
	{
		Ray reflectionViewVector;
		reflectionViewVector.dir = getReflectionVector(viewDirection, 
			getPerturbedNormal(hInfo.N, hInfo.p, reflectionGlossiness));
		reflectionViewVector.p = hInfo.p;
		HitInfo reflectionRayHit;
		reflectionRayHit.Init();
		//--reflection_bounceCount;
		if (TraceRay(&rootNode, reflectionViewVector, reflectionRayHit, HIT_FRONT))
		{
			fromReflection += reflectionRayHit.node->GetMaterial()->Shade(reflectionViewVector, 
				reflectionRayHit, lights, --reflection_bounceCount);
			reflectiveComp = reflectionTotal * fromReflection;
		}
		else
		{
			reflectiveComp = environment.SampleEnvironment(reflectionViewVector.dir);
		}
	}
	/****************************************************************************************************/
	if(GI_ALGO)
	{
		if (kd != noColor && gi_bounceCount > 0)
		{
			HemiSphereSampler giHemiSampler = HemiSphereSampler(__gi_sampleCount, __gi_sampleCount, 1);
			giHemiSampler.generateSamples();
			Point3 randomDirectionAtHitPoint = Point3(0.0f, 0.0f, 0.0f);
			HitInfo diffuseReflectionHitInfo;
			Ray diffuseReflectionRay;
			for (int i = 0; i < giHemiSampler.getCurrentSampleCount(); ++i)
			{
				randomDirectionAtHitPoint = giHemiSampler.getSample(getRandomNumber(0, giHemiSampler.getCurrentSampleCount())).getOffset();
				diffuseReflectionRay.p = hInfo.p;
				Point3 u = Point3(0.0f, 0.0f, 0.0f);
				Point3 v = Point3(0.0f, 0.0f, 0.0f);
				Point3 w = Point3(0.0f, 0.0f, 0.0f);
				w = hInfo.N;
				getOrthoNormalBasisVector(w, u, v);
				diffuseReflectionRay.dir = randomDirectionAtHitPoint.x * u +
					randomDirectionAtHitPoint.y * v +
					randomDirectionAtHitPoint.z *w;
				diffuseReflectionHitInfo.Init();
				if (TraceRay(&rootNode, diffuseReflectionRay, diffuseReflectionHitInfo, HIT_FRONT))
				{
					diffuseReflection = diffuseReflectionHitInfo.node->GetMaterial()->Shade(diffuseReflectionRay,
						diffuseReflectionHitInfo, lights, 0, gi_bounceCount - 1);					
				}
				else
				{
					diffuseReflection = environment.SampleEnvironment(diffuseReflectionRay.dir);
				}
				giHemiSampler.setSampleColor(i, diffuseReflection);
				giHemiSampler.setIsSampleHit(i, true);
			}

			diffuseReflection = giHemiSampler.getAveragedSampleListColor() * diffuseReflection * kd * M_PI;
		}
	}
	else
	{
		if (kd != noColor && gi_bounceCount > 0)
		{
			HemiSphereSampler giHemiSampler = HemiSphereSampler(__gi_sampleCount, __gi_sampleCount, 1);
			giHemiSampler.generateSamples();
			Point3 randomDirectionAtHitPoint = Point3(0.0f, 0.0f, 0.0f);
			HitInfo diffuseReflectionHitInfo;
			Ray diffuseReflectionRay;
			
			randomDirectionAtHitPoint = giHemiSampler.getSample(getRandomNumber(0, giHemiSampler.getCurrentSampleCount())).getOffset();
			diffuseReflectionRay.p = hInfo.p;
			Point3 u = Point3(0.0f, 0.0f, 0.0f);
			Point3 v = Point3(0.0f, 0.0f, 0.0f);
			Point3 w = Point3(0.0f, 0.0f, 0.0f);
			w = hInfo.N;
			getOrthoNormalBasisVector(w, u, v);
			diffuseReflectionRay.dir = randomDirectionAtHitPoint.x * u +
				randomDirectionAtHitPoint.y * v +
				randomDirectionAtHitPoint.z *w;
			
			diffuseReflectionHitInfo.Init();
			if (TraceRay(&rootNode, diffuseReflectionRay, diffuseReflectionHitInfo, HIT_FRONT))
			{
				diffuseReflection += diffuseReflectionHitInfo.node->GetMaterial()->Shade(diffuseReflectionRay,
					diffuseReflectionHitInfo, lights, 0, gi_bounceCount - 1);	
				diffuseReflection = diffuseReflection * kd;
			}
			else
			{
				diffuseReflection = environment.SampleEnvironment(diffuseReflectionRay.dir);
			}			
		}
	}
			
	return (ambientComp + diffuseComp + specularComp+ reflectiveComp + refractiveComp + diffuseReflection);
}

//Russian Roulette implementation - I guess
bool MtlBlinn::RandomPhotonBounce(Ray& r, Color& c, const HitInfo& hInfo) const
{
	float diffuseProbability = 0.0f;
	float reflectionProbability = 0.0f;
	float transmittProbability = 0.0f;
	float absorptionProbability = 0.0f;
	bool photonBounce = false;
	BOUNCE_TYPE photonBounceType = UNKNOWN;

	Color diffuseColor = diffuse.Sample(hInfo.uvw);
	Color refractionColor = refraction.Sample(hInfo.uvw);
	Color reflectionColor = reflection.Sample(hInfo.uvw);

	float averageDiffuse = averageColorComp(diffuseColor);
	float averageReflectance = averageColorComp(reflectionColor);
	float averageTranmittance = averageColorComp(refractionColor);
	float summedAverage = averageDiffuse + averageReflectance + averageTranmittance;
	
	float randomProbability = getRandomNumber(0, 1) * summedAverage;
	
	
	float averageList[] = { averageDiffuse,averageReflectance,averageTranmittance };
	BOUNCE_TYPE bounceList[] = { DIFFUSE_REFLECTION, SPECULAR_REFLECTION, REFRACTION };
	sortLists(averageList, bounceList,3);

	if (randomProbability > 0 && randomProbability <= averageList[0])
		photonBounceType = bounceList[0];
	else if (randomProbability > averageList[0] && randomProbability <= averageList[0] + averageList[1])
		photonBounceType = bounceList[1];
	else if (randomProbability > averageList[0] + averageList[1] && randomProbability <= averageList[2])
		photonBounceType = bounceList[2];
	else
		photonBounceType = ABSORPTION;


	/*if (addColorComp(diffuseColor) > 1.0f)
		diffuseProbability = averageColorComp(diffuseColor);
	else
		diffuseProbability = maxColorComp(diffuseColor);

	if (addColorComp(refractionColor) > 1.0f)
		transmittProbability = averageColorComp(refractionColor);
	else
		transmittProbability = maxColorComp(refractionColor);

	if (addColorComp(reflectionColor) > 1.0f)
		reflectionProbability = averageColorComp(reflectionColor);
	else
		reflectionProbability = maxColorComp(reflectionColor);

	absorptionProbability = 1 - (diffuseProbability + transmittProbability + reflectionProbability);

	float finalProbability = max(diffuseProbability,
		max(transmittProbability, max(reflectionProbability, absorptionProbability)));
		
	if(finalProbability == diffuseProbability)
		photonBounceType = DIFFUSE_REFLECTION;
	if (finalProbability == reflectionProbability)
		photonBounceType = SPECULAR_REFLECTION;
	if (finalProbability == transmittProbability)
		photonBounceType = REFRACTION;
	if (finalProbability == absorptionProbability)
		photonBounceType = ABSORPTION;*/
	
	switch(photonBounceType)
	{
	case DIFFUSE_REFLECTION:
		photonBounce = true;
		r.dir = getRandomDirectionOnHemiSphere(hInfo.N, hInfo.p, 1.0f);
		r.p = hInfo.p;
		c = (c*diffuseColor) / averageDiffuse ;
		break;
	case SPECULAR_REFLECTION:
		photonBounce = true;
		r.dir = getReflectionVector(-r.dir, hInfo.N);
		r.p = hInfo.p;
		c = (c*reflectionColor) / averageReflectance ;
		break;
	case REFRACTION:
		r.dir = getRefractionVector(-r.dir, hInfo.N, 1, ior);
		r.p = hInfo.p;
		c = (c*refractionColor) / averageTranmittance ;
		photonBounce = true;
		break;
	case ABSORPTION:
		photonBounce = false;
		break;
	case UNKNOWN:
		photonBounce = false;
		break;
	}
	return photonBounce;
}

void sortLists(float* i_averageList, BOUNCE_TYPE *i_bounceList, int i_Size)
{
	for (int i = 0; i < i_Size; i++)
	{
		int min = i;
		for (int j = i + 1; j < i_Size; j++)
		{
			if (i_averageList[min] > i_averageList[j])
			{
				min = j;
			}
		}
		if(i !=min)
		{
			float temp = i_averageList[i];
			i_averageList[i] = i_averageList[min];
			i_averageList[min] = temp;
			BOUNCE_TYPE tempBounce = i_bounceList[i];
			i_bounceList[i] = i_bounceList[min];
			i_bounceList[min] = tempBounce;
		}
	}
}

Point3 getRefractionVector(Point3 view, Point3 normal, float n1, float n2)
{
	Point3 Vnormal = normal;
	float cosThetaOne = view.Dot(normal) / view.Length();
	float sinThetaTwo = (n1 / n2) * sqrt(1 - pow(cosThetaOne, 2));
	float cosThetaTwo = sqrt(1 - pow(sinThetaTwo, 2));
	Point3 Vt = (view - (view.Dot(normal)*normal)).GetNormalized();

	/******************Total Internal Reflection **************************/
	//if (sinThetaTwo > 1 || sinThetaTwo < -1) //Checking total interanl reflection
	//	return(getReflectionVector(-view, normal)); //If happened returning a reflection vector
	/*************************End Total Internal Reflection***************/
	return ((cosThetaTwo * -1 * Vnormal) + (sinThetaTwo * -1 * Vt));
}

Point3 getReflectionVector(Point3 view, Point3 normal)
{
	return((2 * normal * (view.Dot(normal))) - view); //Using Reflection Vector for formulae
}

Point3 getPerturbedNormal(Point3 normal, Point3 hitPoint, float i_glossiness)
{
	if (i_glossiness > 0.0f)
	{
		float radius = tanf(i_glossiness*(M_PI / 180));
		SphereSampler sphereSampler = SphereSampler(1, 2, radius);
		sphereSampler.generateSamples();
		Point3 randomOffset = sphereSampler.getSample(getRandomNumber(0, sphereSampler.getCurrentSampleCount())).getOffset();
		Point3 u = Point3(0.0f, 0.0f, 0.0f);
		Point3 v = Point3(0.0f, 0.0f, 0.0f);
		Point3 w = Point3(0.0f, 0.0f, 0.0f);
		w = normal;
		getOrthoNormalBasisVector(w, u, v);
		Point3 samplePoint = randomOffset.x * u + randomOffset.y * v + randomOffset.z * w;
		return (normal + samplePoint).GetNormalized();
	}
	return normal;
}

Point3 getRandomDirectionOnHemiSphere(Point3 normal, Point3 hitPoint, float radius)
{
	/*SphereSampler sphereSampler = SphereSampler(1, 2, radius);
	sphereSampler.generateSamples();
	Point3 randomOffset = sphereSampler.getSample(getRandomNumber(0, sphereSampler.getCurrentSampleCount())).getOffset();
	Point3 u = Point3(0.0f, 0.0f, 0.0f);
	Point3 v = Point3(0.0f, 0.0f, 0.0f);
	Point3 w = Point3(0.0f, 0.0f, 0.0f);
	w = normal;
	getOrthoNormalBasisVector(w, u, v);
	Point3 samplePoint = randomOffset.x * u + randomOffset.y * v + randomOffset.z * w;
	return (normal + samplePoint).GetNormalized();*/

	HemiSphereSampler hemiSphereSampler = HemiSphereSampler(1, 2, radius);
	hemiSphereSampler.generateSamples();
	Point3 randomOffset = hemiSphereSampler.getSample(getRandomNumber(0, hemiSphereSampler.getCurrentSampleCount())).getOffset();
	Point3 u = Point3(0.0f, 0.0f, 0.0f);
	Point3 v = Point3(0.0f, 0.0f, 0.0f);
	Point3 w = Point3(0.0f, 0.0f, 0.0f);
	w = normal;
	getOrthoNormalBasisVector(w, u, v);
	Point3 samplePoint = randomOffset.x * u + randomOffset.y * v + randomOffset.z * w;
	return (normal + samplePoint).GetNormalized();
	//return (hitPoint + randomOffset);
}

float addColorComp(Color i_color)
{
	return (i_color.r + i_color.g + i_color.b);
}

float averageColorComp(Color i_color)
{
	return (addColorComp(i_color) / 3);
}

float maxColorComp(Color i_color)
{
	return max(i_color.r, max(i_color.g, i_color.b));
}

bool MtlBlinn::IsSpecular() const
{
	Color noColor = Color(0.0f);
	if (specular.GetColor() != noColor)
		return true;
	return false;
}







