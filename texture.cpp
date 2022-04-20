#include "texture.h"

solid_color::solid_color(KT::vec3 _color_val)
{
	color_val = _color_val;
}

KT::vec3 solid_color::getColor(float u, float v, const KT::vec3& hitpoint) const
{
	return color_val;
}

KT::vec3 checker_texture::getColor(float u, float v, const KT::vec3& hitpoint) const
{
	float sines = sin(10.0f * hitpoint.m_x) * sin(10.0f * hitpoint.m_y) * sin(10.0f * hitpoint.m_z);
	if (sines < 0) {
		return odd->getColor(u, v, hitpoint);
	} else {
		return even->getColor(u, v, hitpoint);
	}
}



KT::vec3 water_texture::getColor(float u, float v, const KT::vec3& hitpoint) const
{
	
	return KT::vec3();
}
