#pragma once
#include "Geometry/Hittable.h"
#include "Math/Ray.h"
#include "Math/Vec3.h"
#include "Math/MathUtility.h"

class Material
{
public:
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const Vec3& a) : albedo(a) {}
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 target = rec.p + rec.normal + RandomInUnitSphere();
		scattered = Ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}

	Vec3 albedo;
};

class Metal : public Material
{
	public:
	Metal(const Vec3& a, float f) : albedo(a) { if(f < 1) fuzz = f; else fuzz = 1; }
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 reflected = Reflect(r_in.Direction().Normalized(), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * RandomInUnitSphere());
		attenuation = albedo;
		return Vec3::Dot(scattered.Direction(), rec.normal) > 0;
	}

	Vec3 albedo;
	float fuzz;
};

class Dielectric : public Material {
	public:
	Dielectric(float ri) : refIdx(ri) {}
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
		Vec3 outwardNormal;
		Vec3 reflected = Reflect(r_in.Direction(), rec.normal);
		float ni_over_nt;
		attenuation = Vec3(1.f, 1.f, 1.f);
		Vec3 refracted;
		float reflect_prob;
		float cosine;

		if (Vec3::Dot(r_in.Direction(), rec.normal) > 0)
		{
			outwardNormal= -rec.normal;
			ni_over_nt = refIdx;
			cosine = refIdx * Vec3::Dot(r_in.Direction(), rec.normal) / r_in.Direction().Magnitude();
		}
		else {
			outwardNormal = rec.normal;
			ni_over_nt = 1.0f / refIdx;
			cosine = -Vec3::Dot(r_in.Direction(), rec.normal) / r_in.Direction().Magnitude();
		}

		if (Refract(r_in.Direction(), outwardNormal, ni_over_nt, refracted)) {
			reflect_prob = Schlick(cosine, refIdx);
		}
		else {
			reflect_prob = 1.0f;
		}
		
		float random = RandNext();


		if (random < reflect_prob) {
			scattered = Ray(rec.p, reflected);
		}
		else {
			scattered = Ray(rec.p, refracted);
		}
		return true;
	}

	float refIdx; 
};