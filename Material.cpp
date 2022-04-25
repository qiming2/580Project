#include "Material.h"
#include "Tracer.hpp"

namespace KT{
	bool lambertian::scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const {
		vec3 hitpoint = r.eval(rec.m_t);
		vec3 scatter_dir = rec.m_normal + vec3::random_unit_vec3();

		// Since we don't want any degenerated ray like (0.0f, 0.0f, 0.0f)
		if (scatter_dir.near_zero()) {
			scatter_dir = rec.m_normal;
		}

		scattered = ray(hitpoint, scatter_dir, r.time);
		attenuation = albedo->getColor(rec.u, rec.v, hitpoint);
		return true;
	}

	bool metal::scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const {
		vec3 hitpoint = r.eval(rec.m_t);
		vec3 reflected = reflect(r.m_d, rec.m_normal);
		scattered = ray(hitpoint, reflected + fuzzy * vec3::random_unit_sphere(), r.time);
		attenuation = albedo;
		// check whether reflection ray is on the same side of
		// the normal
		return (dot(scattered.m_d, rec.m_normal) > 0.0f);
	}

	bool dielectric::scatter(const ray& r, const Record& rec, vec3& attenuation, ray& scattered) const {
		vec3 hitpoint = r.eval(rec.m_t);
		// no attenuation, glass-like object
		attenuation = vec3(1.0f, 1.0f, 1.0f);

		double index = rec.front_face ? (1.0 / ir) : ir;
		vec3 unit_dir = normalize(r.m_d);

		// need to check whether it must reflect: total internal reflection
		
		double cos_theta = fmin(dot(-unit_dir, rec.m_normal), 1.0f);
		double sin_theta = sqrt(1.0f - cos_theta*cos_theta);

		if (index * sin_theta > 1.0 || reflectance(cos_theta, index) > random_double()) {
			// Must reflect
			vec3 reflect_dir = reflect(unit_dir, rec.m_normal);
			scattered = ray(hitpoint, reflect_dir, r.time);
		}
		else {
			// refract
			
			vec3 refract_dir = refract(unit_dir, rec.m_normal, index);
			scattered = ray(hitpoint, refract_dir, r.time);
		}

		
		

		return true;
	}

	// schlick approximation
	float dielectric::reflectance(float cosine, float index)
	{
		float r0 = (1.0f - index) / (1.0f + index);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * powf(1.0f-cosine, 5);
	}
}