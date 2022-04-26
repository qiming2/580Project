#define _USE_MATH_DEFINES
#include <iostream>
#include "Camera.hpp"



std::ostream& KT::operator<<(std::ostream& out, const KT::Camera& camera)
{
	
	return out;
}

KT::Camera::Camera(vec3 origin, vec3 lookat, vec3 up, float vfov, float aspect_ratio, float aperture, float focus_dist)
{
	float theta = DTOR(vfov);
	float h = tan(theta / 2.0f);
	float viewport_height = 2 * h;
	float viewport_width = viewport_height * aspect_ratio;

	this->origin = origin;
	w = normalize(lookat - origin);
	u = normalize(w.cross(up));
	v = normalize(u.cross(w));
	horizontal = focus_dist * viewport_width * u;
	raw_horizontal = viewport_width * u;

	vertical = focus_dist * viewport_height * v;
	raw_vertical = viewport_height * v;

	lower_left = origin - horizontal / 2.0f - vertical / 2.0f + w * focus_dist;
	raw_lower_left = origin - raw_horizontal / 2.0f - raw_vertical / 2.0f + w * 0.5f;
	lens_radius = aperture / 2.0f;
}

KT::ray KT::Camera::getRay(float s, float t)
{
	vec3 rd = lens_radius * vec3::random_unit_disk();
	vec3 offset = u * rd.m_x + v * rd.m_y;
	KT::ray ret(origin + offset, lower_left + s * horizontal + t * vertical - origin - offset);
	ret.m_d.normalize();
	return ret;
}

KT::vec3 KT::Camera::getRawDir(float s, float t)
{
	vec3 ret = raw_lower_left + s * raw_horizontal + t * raw_vertical - origin;

	return ret;
}


