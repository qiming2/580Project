#define _USE_MATH_DEFINES
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <math.h>
#include "580math.hpp"
#include "Camera.hpp"
#include "Tracer.hpp"
#include "util.hpp"

using namespace KT;
using namespace std;

#define DTOA(x) ((x) / 180.0f * M_PI)

const char* welcomeSen = "This is a ray tracer developed from scratch";
const static int Height = 1024;
const static int Width = 1024;
const static float L = -1.0f;
const static float R = 1.0f;
const static float B = -1.0f;
const static float T = 1.0f;
const static float vfov = 45.0f / 180.0f * M_PI;
const static float near = 1.0f / tan(vfov / 2.0); // make vertical fov 45 degree

// AA param
const static size_t samples_per_pixel = 20;

static KT::Camera c(Width, Height);
vec3 globalDir;
int main() {
	println("Welcome to KTracer Engine");
	println(welcomeSen);
	//print(near);
	// Simple math library
	/*KT::vec3 temp = 3 * vec3(1.0, 2.0, 3);
	KT::vec4 temp1 = KT::vec4(1.0, 2.0, 3, 2.3) * KT::vec4(1.3, 2.5, 4.0, 2);
	KT::ray r;
	KT::mat4 temp3;
	println(temp3);
	println(temp, temp1, r);*/

	// set seed based on time
	srand(time(NULL));
	// TODO:
	// 0. Camera
	vec3 cam_pos(0.0f, 0.0f, 2.0f);
	c.setPos(cam_pos);
	// 1. Generating rays (Only implementing Perspective, orthogonal would be easy if finished perspective)
	vec3 background(0.5, 0.7, 1.0);
	size_t index = 0;
	float r, g, b;
	float* image = new float[Height * Width * 3];
	float t;
	vec3 color;
	for (size_t y = 0; y < Height; ++y) {
		for (size_t x = 0; x < Width; ++x) {
			index = y * Width * 3 + x * 3;
			t = (float)(Height - y) / Height;
			color = (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * background;
			image[index] =     sqrtf(color.m_x);
			image[index + 1] = sqrtf(color.m_y);
			image[index + 2] = sqrtf(color.m_z);
		}
	}
	// ray.m_o = cam pos
	// ray.m_d = u* cam u + v * cam v + -w * cam w
	// w is the distance from cam pos to the center of near plane
	KT::ray cur_ray;
	cur_ray.m_o = c.m_frame.getPos();
	vec3 u = c.m_frame.getU();
	vec3 v = c.m_frame.getV();
	vec3 w = c.m_frame.getW();
	print(c);
	float u_coord;
	float v_coord;

	//Test first ray


	// 2. Generating objects (sphere as the most simplistic object to do the intersection test)
	// Hard code two spheres for testing intersections with SurfaceManager

	KT::Sphere s;
	s.m_o = vec3(0.5f, 0.0f, -near * 0.6);
	s.m_r = 0.3f;

	KT::Sphere s1;
	s1.m_o = vec3(-0.7f, 0.0f, -near * 0.7f);
	s1.m_r = 0.1f;

	KT::Sphere s2;
	s2.m_o = vec3(0.0f, 0.0f, -near * 1.5f);
	s2.m_r = 0.25f;

	KT::Sphere s3;
	s3.m_o = vec3(0.0f, 0.4f, -near * 0.6);
	s3.m_r = 0.2f;

	KT::Sphere s4;
	s4.m_o = vec3(0.0f, -0.5f, -near);
	s4.m_r = 0.5f;

	globalDir = vec3(0.5f, -1.0f, -1.0f);

	SurfaceManager& surf_man = SurfaceManager::getInstance();
	surf_man.Add(s);
	surf_man.Add(s1);
	surf_man.Add(s2);
	surf_man.Add(s3);
	surf_man.Add(s4);

	KT::Triangle tri1;
	tri1.m_a = vec3(-5.0f,- 1.0f, -5.0f);
	tri1.m_b = vec3( 5.0f, -1.0f,  5.0f);
	tri1.m_c = vec3( 5.0f, -1.0f, -5.0f);
	surf_man.Add(tri1);
	KT::Triangle tri2;
	tri2.m_a = vec3( 10.f,  -1.0f,  10.0f);
	tri2.m_b = vec3(-10.f,  -1.0f, -10.0f);
	tri2.m_c = vec3(-10.f,  -1.0f,  10.0f);
	surf_man.Add(tri2);
	/*const static vec3 color = vec3(rand() % 100 / 100.0f / 2.0f + 0.5f, 0.3f, rand() % 100 / 100.0f);*/

	static size_t cnt = 0;
	Record record;
	vec3 hitPoint;


	vec3 output_color;
	// Add a directional light
	static vec3 lightDir = vec3(0.5f, -0.5f, -0.5f);
	lightDir.normalize();
	lightDir = -lightDir;
	const static vec3 lightColor = vec3(0.7f, 0.7f, 0.7f);
	const static vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f);
	float dotH;

	const static float shinness = 32.0f;
	vec3 halfDir;
	float dotHN;
	clock_t start, stop;
	start = clock();
	for (size_t y = 0; y < Height; ++y) {
		for (size_t x = 0; x < Width; ++x) {
      output_color = vec3(0.0f);

      // Antialiasing
      for (size_t s = 0; s < samples_per_pixel; ++s) {
        u_coord = L + ((R - L) * (float)(random_double() +  x)) / Width;
  			v_coord = T - ((T - B) * (float)(random_double() +  y)) / Height;
  			cur_ray.m_d = -near * w + u_coord * u + v_coord * v;
  			cur_ray.m_d.normalize();
  			// Shading inside
  			record = surf_man.intersection(cur_ray, 0, 3, c);
  			if (record.m_surf) {
  				output_color += record.m_color*(1.0f/(float)samples_per_pixel);
        } else {
          output_color += background*(1.0f/(float)samples_per_pixel);
        }

				//print(output_color);
				// Note: y * Width * 3 since we have width * 3 floats in a row!
				index = y * Width * 3 + x * 3;
				image[index] =	   sqrtf(output_color.m_x);
				image[index + 1] = sqrtf(output_color.m_y);
				image[index + 2] = sqrtf(output_color.m_z);
			}
		}
	}
	stop = clock();
	print("CPU rendering took: ", (double)(stop - start) / CLOCKS_PER_SEC);




	// 4. output to a ppm p3 version image for viewing

	// output a simple ppm image
	static bool output = true;
	if (output) {
		const int dimx = Width, dimy = Height;
		ofstream ofs("trace.ppm");
		ofs << "P3" << endl << dimx << " " << dimy << endl << "255" << endl;

		for (size_t y = 0; y < Height; ++y) {
			for (size_t x = 0; x < Width; ++x) {
				// Note: Order matters a lot!!
				// Since we are tracing rays from left to right and top to bottom
				// we need to store in the same order in ppm, otherwise the image might
				// flip upside down
				index = y * Width * 3 + x * 3;
				r = image[index] * 255.0f;
				g = image[index + 1] * 255.0f;
				b = image[index + 2] * 255.0f;
				ofs << r  << " " << g << " " << b << " ";
			}
		}
		ofs.close();

	}
	delete[] image;

	return 0;
}
