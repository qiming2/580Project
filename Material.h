#pragma once
#include "580math.hpp"
// material class holding related params

namespace KT {
	class Record;
	class material
	{
	public:
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const = 0;
	};

	class lambertian : public material {
	public:
		lambertian(const vec3& color) : albedo(color) {}

		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		vec3 albedo;
	};

	class metal : public material {
	public:
		metal(const vec3& color, float f) : albedo(color), fuzzy(f < 1.0f ? f : 1.0f) {}
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		vec3 albedo;
		float fuzzy;
	};

	class dielectric: public material {
	public:
		dielectric(float index_of_refraction) : ir(index_of_refraction) {}
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		float ir;

	private:
		static float reflectance(float cosine, float index);
	};
}


