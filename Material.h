#pragma once
#include "580math.hpp"
#include "texture.h"
// material class holding related params

namespace KT {
	class Record;
	class material
	{
	public:
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const = 0;
		virtual vec3 emit(float u, float v, const vec3& hitpoint) const { return vec3(0.0f); };
	};

	class lambertian : public material {
	public:
		lambertian(const std::shared_ptr<texture> tex) : albedo(tex) {}

		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		std::shared_ptr<texture> albedo;
	};

	class metal : public material {
	public:
		metal(const std::shared_ptr<texture> tex, float f) : albedo(tex), fuzzy(f < 1.0f ? f : 1.0f) {}
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		std::shared_ptr<texture> albedo;
		float fuzzy;
	};

	class dielectric: public material {
	public:
		dielectric(float index_of_refraction) : ir(index_of_refraction), albedo(1.0f) {}
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override;
	public:
		float ir;
		vec3 albedo;
	private:
		static float reflectance(float cosine, float index);
	};

	class emitter : public material {
	public:
		emitter(const std::shared_ptr<texture> tex) : albedo(tex) {}
		virtual bool scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const override { return false; };
		virtual vec3 emit(float u, float v, const vec3& hitpoint) const override{ return albedo->getColor(u, v, hitpoint); };
	private:
		std::shared_ptr<texture> albedo;
	};
}


