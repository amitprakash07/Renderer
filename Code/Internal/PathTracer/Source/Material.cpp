#include "materials.h"
#include "RayIntersection.h"
#include "LightColor.h"
#include "SphereSampler.h"
#include "HemiSphereSampler.h"
#include "utils.h"

#define MONTECARLO_GI 1
#define PATH_TRACING 0
#define GI_ALGO PATH_TRACING

extern Node rootNode;
extern TexturedColor environment;
extern int __gi_sampleCount;
Point3 getRefractionVector(Point3 view, Point3 normal, float n1 = 1, float n2 = 1);
Point3 getReflectionVector(Point3 view, Point3 normal);
Point3 getPerturbedNormal(Point3 normal, Point3 hitPoint, float i_glossiness);

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
		if (lightColor != noColor)
		{
			if (lights[i]->IsAmbient())
			{
				ambientComp += ambientComponent(lights[i], lightColor, hInfo, diffuse.Sample(hInfo.uvw));
			}
			else
			{
				diffuseComp += diffuseComponent(lights[i], lightColor, hInfo, diffuse.Sample(hInfo.uvw));
				specularComp += specularComponent(lights[i], lightColor, viewDirection, hInfo, specular.Sample(hInfo.uvw), glossiness);

			}
		}
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
			refractiveComp = refraction.Sample(hInfo.uvw) * refractionRayHit.node->GetMaterial()->Shade(refractionRay, refractionRayHit, lights, --reflection_bounceCount);
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
		reflectionViewVector.dir = getReflectionVector(viewDirection, getPerturbedNormal(hInfo.N, hInfo.p, reflectionGlossiness));
		reflectionViewVector.p = hInfo.p;
		HitInfo reflectionRayHit;
		reflectionRayHit.Init();
		//--reflection_bounceCount;
		if (TraceRay(&rootNode, reflectionViewVector, reflectionRayHit, HIT_FRONT))
		{
			fromReflection = reflectionRayHit.node->GetMaterial()->Shade(reflectionViewVector, reflectionRayHit, lights, --reflection_bounceCount);
			reflectiveComp += reflectionTotal * fromReflection;
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
				diffuseReflection = diffuseReflectionHitInfo.node->GetMaterial()->Shade(diffuseReflectionRay,
					diffuseReflectionHitInfo, lights, 0, gi_bounceCount - 1);				
			}
			else
			{
				diffuseReflection = environment.SampleEnvironment(diffuseReflectionRay.dir);
			}
			diffuseReflection = diffuseReflection * kd ;
		}
	}
			
	return (ambientComp + diffuseComp + specularComp+ reflectiveComp + refractiveComp + diffuseReflection);
}


Point3 getRefractionVector(Point3 view, Point3 normal, float n1, float n2)
{
	Point3 Vnormal = normal;
	float cosThetaOne = view.Dot(normal)/view.Length();
	float sinThetaTwo = (n1 / n2) * sqrt(1 - pow(cosThetaOne, 2));
	float cosThetaTwo = sqrt(1 - pow(sinThetaTwo, 2));
	Point3 Vt = (view - (view.Dot(normal)*normal)).GetNormalized();
	
	/******************Total Internal Reflection **************************/
	//if (sinThetaTwo > 1 || sinThetaTwo < -1) //Checking total interanl reflection
	//	return(getReflectionVector(-view, normal)); //If happened returning a reflection vector
	/*************************End Total Internal Reflection***************/
	return ((cosThetaTwo * -1 * Vnormal) + (sinThetaTwo * -1 * Vt ));
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
		Point3 randomOffset = sphereSampler.getSample(getRandomNumber(0,sphereSampler.getCurrentSampleCount())).getOffset();
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


