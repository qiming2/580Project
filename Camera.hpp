#pragma once
#include <iostream>
#include "580math.hpp"

namespace KT {
	class Camera {
	public:
		mat4 m_frame;
		float m_width;
		float m_height;
		Camera(const mat4& frame, float width, float height);

		Camera(float width, float height);

		void setPos(const vec3& pos);

		friend std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);
	};
	std::ostream& operator<<(std::ostream& out, const KT::Camera& camera);
}
