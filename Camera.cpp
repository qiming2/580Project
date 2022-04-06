#include <iostream>
#include "Camera.hpp"

KT::Camera::Camera(float width, float height) :
	m_width(width), m_height(height)
{

}

KT::Camera::Camera(const mat4& frame, float width, float height) :
	m_frame(frame), m_width(width), m_height(height)
{

}

void KT::Camera::setPos(const vec3& pos)
{
	m_frame.setPos(pos);
}


//void KT::doSomething(const Camera& camera) {
//	camera.can;
//}

std::ostream& KT::operator<<(std::ostream& out, const KT::Camera& camera)
{
	out << camera.m_frame << " Width: " << camera.m_width << " Height: " << camera.m_height;
	return out;
}
