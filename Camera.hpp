#pragma once
#include <iostream>
#include "580math.hpp"

namespace KT {
	class Camera {
	public:
		Camera(vec3 origin, vec3 lookat, vec3 up, float vfov, float aspect_ratio, float aperture, float focus_dist, double _time1 = 0, double _time2 = 0);

		ray getRay(float u, float v);
		friend std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);

	private:
		vec3 origin;
		vec3 lower_left;
		vec3 horizontal;
		vec3 vertical;
		double time1;
		double time2;
		float lens_radius;
		vec3 u, v, w;
	};
	std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);
}
