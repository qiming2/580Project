#pragma once
#include <iostream>
#include "580math.hpp"

namespace KT {
	class Camera {
	public:
		Camera(vec3 origin, vec3 lookat, vec3 up, float vfov, float aspect_ratio, float aperture, float focus_dist);

		ray getRay(float u, float v);
		friend std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);

	private:
		vec3 origin;
		vec3 lower_left;
		vec3 horizontal;
		vec3 vertical;
		float lens_radius;
		vec3 u, v, w;
	};
	std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);
}
