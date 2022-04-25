#include "texture.h"
#include "580math.hpp"
#include "stb_image.h"

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
	if (output_color[0] < 0.05f) {
		output_color = KT::vec3(1.0f, 1.0f, 1.0f);
	}
	return output_color * albedo;
}

image_texture::image_texture(KT::vec3* _data, int _width, int _height)
{
	data = _data;
	width = _width;
	height = _height;
}

image_texture::~image_texture() {
	delete[] data;
	data = nullptr;
}

image_texture::image_texture(std::string image_dir)
{
	int channels;
	unsigned char* img = stbi_load(image_dir.c_str(), &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image file Name: %s\n", image_dir.c_str());
	} else {
		std::cout << image_dir << " {width: " << width << " height: " << height << " channels: " << channels << "}" << std::endl;
	}

	data = new KT::vec3[width * height];
	//copy data
	size_t index = 0;
	size_t vecIndex = 0;
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			index = i * width * channels + j * channels;
			vecIndex = (height - 1 - i) * width + j;
			data[vecIndex].m_x = (float)img[index] / 255.0f;
			data[vecIndex].m_y = (float)img[index+1] / 255.0f;
			data[vecIndex].m_z = (float)img[index+2] / 255.0f;
		}
	}
	stbi_image_free(img);
}

KT::vec3 image_texture::getColor(float u, float v, const KT::vec3& hitpoint) const
{
	KT::vec3 color = (0, 0, 0);
	float x = u * (width - 1);
	float y = v * (height - 1);

	float s = x - floor(x);
	float t = y - floor(y);
	for (int c = 0; c < 3; c++) {
		color[c] = s * t * data[int(ceil(y) * width + ceil(x))][c] + (1 - s) * t * data[int(ceil(y) * width + floor(x))][c] + s * (1 - t) * data[int(floor(y) * width + ceil(x))][c] + (1 - s) * (1 - t) * data[int(floor(y) * width + floor(x))][c];
	}

	return color;
}

// windDir should be vec3(x, y, 0); n is the size of the output image texture
KT::vec3* genGersterWaveTexture(KT::vec3 windDir, int n)
{
	// Use the Texture on a 10m basis
	float A = KT::random_double(0.01f, 0.02f);
	float Q = KT::random_double(0.3f, 0.4f);
	// The wave direction is determined by wind direction
	// but have a random angle to the wind direction
	float windAngle = acosf((windDir.m_x / sqrtf(windDir.m_x * windDir.m_x + windDir.m_y * windDir.m_y)));
	if (windDir.m_y < 0) windAngle = -windAngle;
	float waveAngle = KT::random_double(windAngle - DTOR(45.0f),
		windAngle + DTOR(45.0f));
	KT::vec3 waveD = (cos(waveAngle), sin(waveAngle), 0);
	float s = KT::random_double(0.5f, 1.0f);
	float l = ((float)KT::random_int(4, 32) / n);


	KT::vec3* data = new KT::vec3[n * n];
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			data[i * n + j] = KT::vec3(0.0f, 1.0f, 0.0f);
			//no wave animation, so time = 0;
			float u = (float)j / n, v = (float)i / n;
			float w = 2 * M_PI / l;
			//float fi = 2 * M_PI * s / l;

			data[i * n + j].m_x += (-waveD.m_x) * w * A * cos(waveD.dot(KT::vec3(u, v, 0)) * w);
			data[i * n + j].m_y -= Q * w * A * sin(waveD.dot(KT::vec3(u, v, 0)) * w);
			data[i * n + j].m_z += (-waveD.m_y) * w * A * cos(waveD.dot(KT::vec3(u, v, 0)) * w);
			//std::cout << (-waveD.m_x) * w * A * cos(waveD.dot(KT::vec3(u, v, 0)) * w) << std::endl;
		}
	}
	return data;
}
