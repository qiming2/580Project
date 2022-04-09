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


const char* welcomeSen = "This is a ray tracer developed from scratch";
const static int Height = 1024;
const static int Width = 1024;
const static float L = -1.0f;
const static float R = 1.0f;
const static float B = -1.0f;
const static float T = 1.0f;
const static float vfov = 45.0f / 180.0f * M_PI;
const static float near = 1.0f / tan(vfov / 2.0); // make vertical fov 45 degree

//////////////////////////////////// rewrite generate rays ////////////////////////

// image param
const float aspect_ratio = 3.0f / 2.0f;
const int image_width = 1200;
const int image_height = (int)((float)image_width / aspect_ratio);

// Camera
float viewport_height = 2.0f;
float viewport_width = viewport_height * aspect_ratio;
float focal_length = 1.0f;

vec3 origin = vec3(0.0f, 0.0f, 0.0f);
vec3 horizontal = vec3(viewport_width, 0.0f, 0.0f);
vec3 vertical = vec3(0.0f, viewport_height, 0.0f);
vec3 lower_left = origin - horizontal / 2.0f - vertical / 2.0f - vec3(0.0f, 0.0f, focal_length);

size_t max_depth = 20;


// AA param
const static size_t samples_per_pixel = 20;
///////////////////////////////////////////////////////////////////////////////

using namespace std;

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
	/*KT::ray cur_ray;
	cur_ray.m_o = c.m_frame.getPos();
	vec3 u = c.m_frame.getU();
	vec3 v = c.m_frame.getV();
	vec3 w = c.m_frame.getW();
	print(c);
	float u_coord;
	float v_coord;*/

	//Test first ray


	// 2. Generating objects (sphere as the most simplistic object to do the intersection test)
	// Hard code two spheres for testing intersections with SurfaceManager

	SurfaceManager& surf_man = SurfaceManager::getInstance();
	
	surf_man.make_random_scene();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0f;
	auto aperture = 0.1f;
	Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);



	
	//surf_man.Add(s2);
	//surf_man.Add(s3);
	//surf_man.Add(s4);

	/*KT::Triangle tri1;
	tri1.m_a = vec3(-5.0f,- 1.0f, -5.0f);
	tri1.m_b = vec3( 5.0f, -1.0f,  5.0f);
	tri1.m_c = vec3( 5.0f, -1.0f, -5.0f);
	surf_man.Add(tri1);
	KT::Triangle tri2;
	tri2.m_a = vec3( 10.f,  -1.0f,  10.0f);
	tri2.m_b = vec3(-10.f,  -1.0f, -10.0f);
	tri2.m_c = vec3(-10.f,  -1.0f,  10.0f);
	surf_man.Add(tri2);*/
	/*const static vec3 color = vec3(rand() % 100 / 100.0f / 2.0f + 0.5f, 0.3f, rand() % 100 / 100.0f);*/


	//globalDir = vec3(0.5f, -1.0f, -1.0f);
	//static size_t cnt = 0;
	//
	//vec3 hitPoint;


	//
	//// Add a directional light
	//static vec3 lightDir = vec3(0.5f, -0.5f, -0.5f);
	//lightDir.normalize();
	//lightDir = -lightDir;
	//const static vec3 lightColor = vec3(0.7f, 0.7f, 0.7f);
	//const static vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f);
	//float dotH;

	//const static float shinness = 32.0f;
	//vec3 halfDir;
	//float dotHN;
	Record record;
	vec3 output_color;
	clock_t start, stop;
	start = clock();
	for (size_t y = 0; y < image_height; ++y) {
		for (size_t x = 0; x < image_width; ++x) {
      output_color = vec3(0.0f);
	  
	  // progress bar
	  
	  //std::cerr << "Progress: " << y * image_width + x+1 << " out of " << image_height * image_width << std::flush;
      // Antialiasing
      for (size_t s = 0; s < samples_per_pixel; ++s) {
		  ray cur_ray;
		  float v = (float(y) + random_double()) / (float)(image_height - 1);
		  float u = (float(x) + random_double()) / (float)(image_width - 1);
		  cur_ray = cam.getRay(u, v);
  		// Shading inside
  		record = surf_man.intersection(cur_ray, 0, max_depth, cam);
  		if (record.m_surf) {
  			output_color += record.m_color*(1.0f/(float)samples_per_pixel);
        } else {
          output_color += background*(1.0f/(float)samples_per_pixel);
        }

				//print(output_color);
				// Note: y * Width * 3 since we have width * 3 floats in a row!
				index = y * image_width * 3 + x * 3;
				// gamma correction
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
		const int dimx = image_width, dimy = image_height;
		ofstream ofs("trace.ppm");
		ofs << "P3" << endl << dimx << " " << dimy << endl << "255" << endl;

		for (int y = image_height - 1; y >= 0 ; --y) {
			for (int x = 0; x < image_width; ++x) {
				// Note: Order matters a lot!!
				// Since we are tracing rays from left to right and top to bottom
				// we need to store in the same order in ppm, otherwise the image might
				// flip upside down
				index = y * image_width * 3 + x * 3;
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
