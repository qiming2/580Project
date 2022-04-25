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
	float sines = sin(100.0f * hitpoint.m_x) * sin(100.0f * hitpoint.m_y) * sin(100.0f * hitpoint.m_z);
	int x = (int)(u * 2.0f * 10.0f);
	int y = (int)(v * 10.0f);
	if ((x+y)%2 == 0) {
		return odd->getColor(u, v, hitpoint);
	}
	else {
		return even->getColor(u, v, hitpoint);
	}

	/*if (sines < 0) {
		return odd->getColor(u, v, hitpoint);
	} else {
		return even->getColor(u, v, hitpoint);
	}*/
}


// cellular noise
struct point {
	float x, y;

	point() {}
	point(float _x, float _y) {
		x = _x;
		y = _y;
	}
	float dot(const point& other) {
		return x * other.x + y * other.y;
	}

	point operator-(const point& other) {
		point ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}

	point operator+(const point& other) {
		point ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}

	point operator*(const float val) {
		point ret;
		ret.x = x * val;
		ret.y = y * val;
		return ret;
	}

	point operator/(const float val) {
		point ret;
		ret.x = x / val;
		ret.y = y / val;
		return ret;
	}

	float len() {
		return sqrtf(x * x + y * y);
	}

	point floor() {
		point ret;
		ret.x = floorf(x);
		ret.y = floorf(y);
		return ret;
	}

	point fract() {
		point ret;
		ret.x = x - floorf(x);
		ret.y = y - floorf(y);
		return ret;
	}

	float dist(const point& other) {
		return (*this - other).len();
	}
};


// pseudo random function
point random2(point p) {
	point ret;
	ret.x = sinf(p.dot(point(137.1, 25.7)));
	ret.y = sinf(p.dot(point(72.5, 137.3)) * 23244.3123);
	return ret.fract();
}

#define DIM 10.0f
KT::vec3 water_texture::getColor(float u, float v, const KT::vec3& hitpoint) const
{
	// tile the space into subspaces to
	// get more cells and make it more performant
	point cur;
	cur.x = u;
	cur.y = v;
	cur = cur * DIM;

	point i_st = cur.floor();
	point f_st = cur.fract();

	float m_dist = 1.0f;
	float cur_dist;
	point min_p;

	point neighbor;
	// we only need to check the adjacent random cell
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			neighbor.x = float(x);
			neighbor.y = float(y);

			// get a random point in the current neightbor cell
			point p = i_st + neighbor;
			if (p.x < 0.0f) p.x = DIM;
			if (p.y < 0.0f) p.y = DIM;
			if (p.x > DIM) p.x = 0.0f;
			if (p.y > DIM) p.y = 0.0f;
			point neighP = random2(p);

			// we can use sin wave to animate the cell
			
			neighP.x = 0.5 + 0.5 * sin((double)neighP.x * 2 * 3.1415926);
			neighP.y = 0.5 + 0.5 * sin((double)neighP.y * 2 * 3.1415926);

			point distanceV = neighP + neighbor - f_st;
			cur_dist = distanceV.len();

			if (cur_dist < m_dist) {
				m_dist = cur_dist;
				//min_p = neighP + neighbor;
			}
		}
	}
	KT::vec3 output_color(0.0f);
	/*output_color[0] = 1.0f - m_dist;
	output_color[1] = 1.0f - m_dist;
	output_color[2] = 1.0f - m_dist;

	output_color[0] -= abs(sin(40.0 * m_dist / 2)) * 0.1;
	output_color[1] -= abs(sin(40.0 * m_dist / 2)) * 0.1;
	output_color[2] -= abs(sin(40.0 * m_dist / 2)) * 0.1;*/
	
	output_color[0] = 1.0f - m_dist;
	output_color[1] = 1.0f - m_dist;
	output_color[2] = 1.0f - m_dist;
	if (output_color[0] < 0.02f) {
		output_color = KT::vec3(1.0f, 1.0f, 1.0f);
	}
	return output_color * albedo;
}

KT::vec3 image_texture::getColor(float u, float v, const KT::vec3& hitpoint) const
{

	return KT::vec3();
}