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
#include "stb_image.h"
#include "stb_image_write.h"
#include "texture.h"

using namespace KT;
using namespace std;


const char* welcomeSen = "This is a ray tracer developed from scratch";

//////////////////////////////////// params ////////////////////////

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
const static size_t samples_per_pixel = 10;
///////////////////////////////////////////////////////////////////////////////

using namespace std;

vec3 globalDir;
int main() {
	println("Welcome to KTracer Engine");
	println(welcomeSen);

	// set seed based on time
	srand(time(NULL));

	// 0. Camera

	// 1. Generating rays (Only implementing Perspective, orthogonal would be easy if finished perspective)
	vec3 background(0.0, 0.7, 1.0);
	size_t index = 0;
	float r, g, b;
	float* image = new float[image_height *image_width * 3];
	uint8_t* image_out = new uint8_t[image_height * image_width * 3];
	float t;
	vec3 color;
	for (size_t y = 0; y < image_height; ++y) {
		for (size_t x = 0; x < image_width; ++x) {
			index = y * image_width * 3 + x * 3;
			t = (float)(image_height - y) / image_height;
			color = (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * background;
			image[index] =     sqrtf(color.m_x);
			image[index + 1] = sqrtf(color.m_y);
			image[index + 2] = sqrtf(color.m_z);
		}
	}

	// 2. Generating objects (sphere as the most simplistic object to do the intersection test)

	//////////////////////////////// Test Scene End /////////////////

	SurfaceManager& surf_man = SurfaceManager::getInstance();
	surf_man.set_env_map("Texture/Lycksele/");
	//surf_man.make_random_scene();
	auto usc_tex = make_shared<image_texture>("Texture/usc1.png");
	auto lam_tex = make_shared<solid_color>(vec3(1.0, 1.0, 1.0));
	std::shared_ptr<texture> water_color = make_shared<solid_color>(vec3(212.0f / 255.0f, 241.0f / 255.0f, 249.0f / 255.0f));
	auto material4 = make_shared<metal>(water_color, 0.0);
	std::shared_ptr<material> light_mat = make_shared<emitter>(lam_tex);
	auto water_material = make_shared<dielectric>(1.33);
	water_material->albedo = vec3(1.0f);
	std::shared_ptr<texture> water_tex = make_shared<water_texture>();
	std::shared_ptr<texture> checker = make_shared<checker_texture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
	auto material2 = make_shared<lambertian>(water_tex);
	auto material3 = make_shared<lambertian>(usc_tex);
	surf_man.Add(make_shared<Sphere>(vec3(-2.5, 1, 0), 1.0, material3));

	surf_man.Add(make_shared<Sphere>(vec3(2.5, 1, 0), 1.0, material4));

	surf_man.Add(make_shared<Sphere>(vec3(0, 1, 0), 1.0, light_mat));
	float length = 30;
	vec3 ta = { -length , -1 ,  length };
	vec3 tb = { length  , -1,  length };
	vec3 tc = { -length , -1 , -length };

	vec3 td = { length  , -1, -length };
	auto tri1 = make_shared<Triangle>(tb, ta, tc);

	tri1->m_uv_data[0] = { 0, 0, 0 };
	tri1->m_uv_data[1] = { 0, 1, 0 };
	tri1->m_uv_data[2] = { 1, 0, 0 };
	//tri1->mat_ptr = material3;
	tri1->mat_ptr = water_material;
	auto tri2 = make_shared<Triangle>(tc, td, tb);
	tri2->m_uv_data[0] = { 1, 1, 0 };
	tri2->m_uv_data[1] = { 1, 0, 0 };
	tri2->m_uv_data[2] = { 0, 1, 0 };
	//tri2->mat_ptr = material3;
	tri2->mat_ptr = water_material;

	size_t texCount = 8;
	tri1->normap = new std::shared_ptr<texture>[texCount];
	tri2->normap = new std::shared_ptr<texture>[texCount];
	for (int i = 0; i < texCount; i++) {
		vec3* normap = genGersterWaveTexture(vec3(KT::random_double(0, 1), KT::random_double(0, 1), 0), 100);
		std::shared_ptr<texture> normap_texture = make_shared<image_texture>(normap, 100, 100);
		tri1->normap[i] = normap_texture;
		tri2->normap[i] = normap_texture;
	}

	surf_man.Add(tri1);
	surf_man.Add(tri2);


	//surf_man.Add(make_shared<Sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	/*unsigned char* img = stbi_load(temp_src_path.c_str(), &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image file Name: %s\n", temp_src_path.c_str());
	}*/


	//////////////////////////////// Test Scene End /////////////////

	vec3 lookfrom(0, 4, -10);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = (lookfrom - lookat).len();
	auto aperture = 0.1f;
	Camera cam(lookfrom, lookat, vup, 45.0f, aspect_ratio, aperture, dist_to_focus);

	Record record;
	vec3 output_color;
	clock_t start, stop;
	start = clock();

	/////////////////////////////////////// Ray Trace Core ////////////////////////////////////////////
	surf_man.construct_BVH();
	surf_man.have_background = false;
	for (size_t y = 0; y < image_height; ++y) {
		for (size_t x = 0; x < image_width; ++x) {
      output_color = vec3(0.0f);

      // Antialiasing
      for (size_t s = 0; s < samples_per_pixel; ++s) {
		  ray cur_ray;
		  float v = (float(y) + random_double()) / (float)(image_height - 1);
		  float u = (float(x) + random_double()) / (float)(image_width - 1);
		  cur_ray = cam.getRay(u, v);
  			// Shading inside
  			record = surf_man.intersection(cur_ray, 0, max_depth, cam);
			if (record.m_surf) {
				output_color += record.m_color * (1.0f / (float)samples_per_pixel);

			}
			// Sampling environment map
			else {
				vec3 sample_dir = cam.getRawDir((float(x) + 0.5f) / (float)(image_width - 1), (float(y) + 0.5f) / (float)(image_height - 1));
				output_color += surf_man.getEnvColor(sample_dir) * (1.0f / (float)samples_per_pixel);
			}


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
	stop = clock();
	print("CPU rendering took: ", (double)(stop - start) / CLOCKS_PER_SEC);




	// 4. output to a ppm p3 version image for viewing

	// output a simple ppm image
	static bool output = true;
	if (output) {
		int channels;
		int width, height;
		const int dimx = image_width, dimy = image_height;
		ofstream ofs("trace.ppm");
		ofs << "P3" << endl << dimx << " " << dimy << endl << "255" << endl;
		int out_index = 0;
		for (int y = image_height - 1; y >= 0 ; --y) {
			for (int x = 0; x < image_width; ++x) {
				// Note: Order matters a lot!!
				// Since we are tracing rays from left to right and top to bottom
				// we need to store in the same order in ppm, otherwise the image might
				// flip upside down
				index = y * image_width * 3 + x * 3;
				out_index = (image_height - 1 - y) * image_width * 3 + x * 3;
				r = image[index] * 255.0f;
				g = image[index + 1] * 255.0f;
				b = image[index + 2] * 255.0f;
				image_out[out_index] = uint8_t(r);
				image_out[out_index +1] = uint8_t(g);
				image_out[out_index +2] = uint8_t(b);
				ofs << r  << " " << g << " " << b << " ";
			}
		}

		ofs.close();
		if (stbi_write_jpg("trace.jpg", image_width, image_height, 3, image_out, 100) == 0) {
			printf("Error in writing the image");
		}

	}
	delete[] image;

	return 0;
}
