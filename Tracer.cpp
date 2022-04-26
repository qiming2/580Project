#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "util.hpp"
#include "texture.h"
#include "Tracer.hpp"
#include <algorithm>
#define USE_BVH 1

namespace KT {
	bool AABB::hit(const KT::ray& r) const
	{
		// check three axises
		float t_min = -std::numeric_limits<float>::max();
		float t_max = std::numeric_limits<float>::max();
		for (size_t i = 0; i < 3; ++i) {
			float invD = 1.0f / r.m_d[i];
			float t0 = (minP[i] - r.m_o[i]) * invD;
			float t1 = (maxP[i] - r.m_o[i]) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			t_min = fmax(t0, t_min);
			t_max = fmin(t1, t_max);
			if (t_max <= t_min) return false;
		}

		return true;
	}


	// Main BVH_node creation
	BVH_Node::BVH_Node(std::vector<std::shared_ptr<Surface>> lists, size_t start, size_t end)
	{
		
		int axis = random_int(0, 2);
		

		// comparator declaration
		bool (*comparator)(const std::shared_ptr<Surface> a, const std::shared_ptr<Surface> b);

		comparator = (axis == 0) ? box_x_compare
			: (axis == 1) ? box_y_compare
			: box_z_compare;

		size_t surface_range = end - start;

		// special cases
		if (surface_range == 0) {
			box = AABB(vec3(1.0f), vec3(-1.0f));
			return;
		}
		else if (surface_range == 1) {
			left = right = lists[start];
		}
		else if (surface_range == 2) {
			left = lists[start];
			right = lists[start + 1];
		}
		else {
			// sort the range of surfaces depending selected axis
			std::sort(lists.begin() + start, lists.begin() + end, comparator);
			
			// split into half for left and right
			size_t mid = start + (end - start) / 2;
			left = std::make_shared<BVH_Node>(lists, start, mid);
			right = std::make_shared<BVH_Node>(lists, mid, end);
		}

		AABB lb, rb;

		if (!left->bbox(lb) || !right->bbox(rb)) {
			std::cerr << "NO BOUNDING BOX" << std::endl;
		}
		
		box = enclose_box(lb, rb);
	}



	Record BVH_Node::intersection(const ray& r) const
	{
		Record ret;
		if (!box.hit(r)) return ret;

		ret = left->intersection(r);
		Record intersect_right = right->intersection(r);
		if (ret.m_surf && intersect_right.m_surf) {
			return ret.m_t < intersect_right.m_t ? ret : intersect_right;
		}

		if (intersect_right.m_surf) {
			return intersect_right;
		}
		else {
			return ret;
		}
	}

	bool BVH_Node::bbox(AABB& out_box) const
	{
		out_box = box;
		return true;
	}

}
KT::Record::Record(Surface* surf, const vec3& norm, float t) :m_surf(surf), m_normal(norm), m_t(t)
{

}

KT::Record::Record():m_t(std::numeric_limits<float>::infinity())
{

}


KT::Record::Record(Surface* surf, float t) : m_surf(surf), m_t(t)
{

}

void KT::Sphere::getUV(const KT::vec3& p, float& u, float& v) {
	float theta = acos(-p.m_y);
	float phi = atan2(-p.m_z, p.m_x) + M_PI;

	u = phi / (2 * M_PI);
	v = theta / M_PI;
}

KT::Record KT::Sphere::intersection(const ray& r) const
{
	Record record;
	record.mat_ptr = mat_ptr;
	vec3 e_minus_c = r.m_o - m_o;
	float d_dot_d = r.m_d.dot(r.m_d);
	float e_dot_d = r.m_d.dot(e_minus_c);
	float discrim = e_dot_d * e_dot_d - d_dot_d * ((e_minus_c.dot(e_minus_c) - m_r * m_r));
	if (discrim < EPSILON) {
		return record;
	}

	discrim = sqrtf(discrim);
	vec3 point;
	float t1 = (-e_dot_d - discrim) / d_dot_d;
	if (t1 > HIT_EPSILON) {
		record.m_surf = this;
		record.m_t = t1;

		point = r.eval(t1);

		record.m_normal = (point - m_o) / m_r;
		record.set_face_normal(r, record.m_normal);
		Sphere::getUV(record.m_normal, record.u, record.v);
		//println(record.m_normal);
		return record;
	}

	float t2 = (-e_dot_d + discrim) / d_dot_d;
	if (t2 > HIT_EPSILON) {
		record.m_surf = this;
		record.m_t = t2;

		point = r.eval(t2);

		record.m_normal = (point - m_o) / m_r;
		record.set_face_normal(r, record.m_normal);
		Sphere::getUV(record.m_normal, record.u, record.v);
		return record;
	}

	return record;
}

bool KT::Sphere::bbox(KT::AABB& out_box) const
{
	out_box = KT::AABB(
		m_o - vec3(m_r, m_r, m_r),
		m_o + vec3(m_r, m_r, m_r)
	);
	return true;
}

std::ostream& KT::operator<<(std::ostream& out, const KT::Sphere& s)
{
	out << "Origin: " << s.m_o << " Radius: " << s.m_r;
	return out;
}

KT::SurfaceManager& KT::SurfaceManager::getInstance()
{
	static SurfaceManager instance;
	return instance;
}

void KT::SurfaceManager::Add(std::shared_ptr<Surface> surf)
{
	surfaces.push_back(surf);
}

void KT::SurfaceManager::make_random_scene()
{
	using namespace std;
	using namespace KT;
	using color = vec3;
	using point3 = vec3;
	surfaces.clear();

	std::shared_ptr<texture> ground = make_shared<solid_color>(color(0.5, 0.5, 0.5));
	auto ground_material = make_shared<lambertian>(ground);
	Add(make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));


	for (int a = -5; a < 5; a++) {
		for (int b = -5; b < 5; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).len() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					std::shared_ptr<texture> plain_tex = make_shared<solid_color>(albedo);

					sphere_material = make_shared<lambertian>(plain_tex);
					Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = make_shared<solid_color>(color::random(0.5, 1));
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}
	auto material1 = make_shared<dielectric>(1.5);
	Add(make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

	auto lam_tex = make_shared<solid_color>(vec3(0.4, 0.2, 0.1));
    auto material2 = make_shared<lambertian>(lam_tex);
    Add(make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

	auto metal_tex = make_shared<solid_color>(vec3(0.7, 0.6, 0.5));
    auto material3 = make_shared<metal>(metal_tex, 0.0);
    Add(make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

}

extern KT::vec3 globalDir;
KT::Record KT::SurfaceManager::intersection(const ray& r, size_t level, size_t max_level, const Camera& c)
{
	Record ret;
	ret.m_color = vec3(0.0f);
	if (level == max_level) return ret;


	

#if USE_BVH
	//clock_t start, stop;
	//start = clock();
	ret = cur.intersection(r);
	//stop = clock();
	//print("One Ray took: ", (double)(stop - start) / CLOCKS_PER_SEC);
#else 
	clock_t start, stop;
	start = clock();
	Record record;

	for (size_t i = 0; i < surfaces.size(); ++i) {
		record = surfaces[i]->intersection(r);
		if (record.m_surf && ret.m_t > record.m_t - EPSILON) {
			ret = record;
		}
	}
	stop = clock();
	print("One Ray took: ", (double)(stop - start) / CLOCKS_PER_SEC);
#endif

	if (ret.m_surf) {
		ray scattered;
		vec3 attenuation;
		// Triangle
		if (!ret.mat_ptr) {
			vec3 hitpoint = r.eval(ret.m_t);
			vec3 gen_color = vec3(1.0, 1.0, 1.0);
			//ret.m_color = gen_color * intersection(ray(hitpoint, reflect(r.m_d, ret.m_normal)), level + 1, max_level, c).m_color;

			//ret.m_color = vec3(212.0f / 255.0f, 241.0f / 255.0f, 249.0f / 255.0f);
			return ret;
		} else if (ret.mat_ptr->scatter(r, ret, attenuation, scattered)) {
			vec3 hitpoint = r.eval(ret.m_t);
			ret.m_color = ret.mat_ptr->emit(ret.u, ret.v, hitpoint);
			ret.m_color += attenuation * intersection(scattered, level + 1, max_level, c).m_color;
			return ret;
		}

		vec3 hitpoint = r.eval(ret.m_t);
		ret.m_color = ret.mat_ptr->emit(ret.u, ret.v, hitpoint);
		return ret;
	}

	// background color sampling environment map
	vec3 unitDir = normalize(r.m_d);
	if (env_map) {
		ret.m_color = env_map->getColor(unitDir);
	}
	else {
		float t = 0.5f * (unitDir.m_y + 1.0f);
		ret.m_color = (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
	

	
	return ret;
}

void KT::SurfaceManager::construct_BVH()
{
	cur = BVH_Node(surfaces, 0, surfaces.size());
}

void KT::SurfaceManager::set_env_map(const std::string& dir)
{
	env_map = std::make_shared<cube_map>(dir);
}

KT::vec3 KT::SurfaceManager::getEnvColor(const KT::vec3& dir)
{
	if (env_map) {
		return env_map->getColor(dir);
	}

	return KT::vec3(0.0f);
}

bool KT::SurfaceManager::bbox(AABB& out_box)
{
	if (surfaces.empty()) return false;

	AABB cur_box;
	bool first = true;

	for (std::shared_ptr<Surface>& surface : surfaces) {
		// Edge cases
		if (!surface->bbox(cur_box)) return false;
		out_box = first ? cur_box : enclose_box(out_box, cur_box);
		first = false;
	}

	return true;
}

KT::SurfaceManager::SurfaceManager()
{

}

KT::AABB KT::enclose_box(const AABB& b0, const AABB& b1)
{
	vec3 small = vec3(
		fmin(b0.minP[0], b1.minP[0]),
		fmin(b0.minP[1], b1.minP[1]),
		fmin(b0.minP[2], b1.minP[2])
	);

	vec3 big = vec3(
		fmax(b0.maxP[0], b1.maxP[0]),
		fmax(b0.maxP[1], b1.maxP[1]),
		fmax(b0.maxP[2], b1.maxP[2])
	);
	return AABB(small, big);
}

// This version is more prone to mistakes considering there
// are so many floats involved in the operations
//KT::Record KT::Triangle::intersection(const ray& r) const
//{
//	// Cramer's rule
//	// point a - point b
//	float xa_minus_xb = m_a.m_x - m_b.m_x;
//	float ya_minus_yb = m_a.m_y - m_b.m_y;
//	float za_minus_zb = m_a.m_z - m_b.m_z;
//
//	vec3 v0v1 = m_b - m_a;
//	vec3 v0v2 = m_c - m_a;
//	vec3 pvec = r.m_d.cross(v0v2);
//	float determ = v0v1.dot(pvec);
//
//	// point a - point c
//	float xa_minus_xc = m_a.m_x - m_c.m_x;
//	float ya_minus_yc = m_a.m_y - m_c.m_y;
//	float za_minus_zc = m_a.m_z - m_c.m_z;
//
//	// point a - point e
//	float xa_minus_xe = m_a.m_x - r.m_o.m_x;
//	float ya_minus_ye = m_a.m_y - r.m_o.m_y;
//	float za_minus_ze = m_a.m_z - r.m_o.m_z;
//
//	float ei_minus_hf = ya_minus_yc * r.m_d.m_z - za_minus_zc * r.m_d.m_y;
//	float gf_minus_di = r.m_d.m_x * za_minus_zc - xa_minus_xc * r.m_d.m_z;
//	float dh_minus_eg = xa_minus_xc * r.m_d.m_y - ya_minus_yc * r.m_d.m_x;
//
//	Record ret;
//	float determ = xa_minus_xb * ei_minus_hf + ya_minus_yb * gf_minus_di + za_minus_zb * dh_minus_eg;
//	if (determ == 0.0f) {
//		//print("somthing : t");
//		return ret;
//	}
//	float ak_minus_jb = xa_minus_xb * ya_minus_ye - xa_minus_xe * ya_minus_yb;
//	float jc_minus_al = xa_minus_xe * za_minus_zb - xa_minus_xb * za_minus_ze;
//	float bl_minus_kc = ya_minus_yb * za_minus_ze - ya_minus_ye * za_minus_zb;
//	float t = -(za_minus_zc * ak_minus_jb + ya_minus_yc * jc_minus_al + xa_minus_xc * bl_minus_kc) / determ;
//	if (t < EPSILON) return ret;
//	ret.m_t = t;
//
//	float alpha = (r.m_d.m_z * ak_minus_jb + r.m_d.m_y * jc_minus_al + r.m_d.m_x * bl_minus_kc) / determ;
//	if (alpha < EPSILON || alpha > 1.0f) return ret;
//
//	float beta = (xa_minus_xe * ei_minus_hf + ya_minus_ye * gf_minus_di + xa_minus_xe * dh_minus_eg) / determ;
//
//	if (beta < EPSILON || beta > 1.0f) return ret;
//
//	ret.m_surf = this;
//	ret.m_normal.m_x = ya_minus_yb * za_minus_zc - ya_minus_yc * za_minus_zb;
//	ret.m_normal.m_y = za_minus_zb * xa_minus_xc - xa_minus_xb * za_minus_zc;
//	ret.m_normal.m_z = xa_minus_xb * ya_minus_yc - ya_minus_yb * xa_minus_xc;
//	ret.m_normal.normalize();
//	//print("Something?");
//	return ret;
//}


// The smart version
KT::Record KT::Triangle::intersection(const ray& r) const
{
	vec3 v0v1 = m_b - m_a;
	vec3 v0v2 = m_c - m_a;
	vec3 pvec = r.m_d.cross(v0v2);
	float determ = v0v1.dot(pvec);
	Record ret;

	// Culling: parallel
	if (determ > -EPSILON && determ < EPSILON) return ret;
	// Backface culling
	//if (determ < EPSILON) return ret;
	float invDet = 1.0f / determ;
	vec3 tvec = r.m_o - m_a;

	float u = tvec.dot(pvec) * invDet;
	if (u < 0.0f || u > 1.0f) return ret;

	vec3 qvec = tvec.cross(v0v1);
	float v = r.m_d.dot(qvec) * invDet;
	if (v < 0.0f || u + v > 1.0f)  return ret;

	float t = v0v2.dot(qvec) * invDet;
	if (t < HIT_EPSILON) return ret;


	ret.m_t = t;
	ret.m_surf = this;
	vec3 hitpoint = r.eval(ret.m_t);
	ret.mat_ptr = mat_ptr;
	ret.u = ((1.0f - u - v) * m_uv_data[2][0] + u * m_uv_data[0][0] + v * m_uv_data[1][0]);
	ret.v = ((1.0f - u - v) * m_uv_data[2][1] + u * m_uv_data[0][1] + v * m_uv_data[1][1]);
	ret.m_color = vec3(ret.u, ret.v, 0.0f);
	//ret.m_normal = v0v2.cross(v0v1);
	for (int i = 0; i < 8; i++) {
		ret.m_normal += normap[i]->getColor(ret.u, ret.v, hitpoint);
	}
	ret.m_normal.normalize();
	/*ret.m_color = ret.m_normal;*/
	return ret;
}

bool KT::Triangle::bbox(AABB& out_box) const
{
	vec3 minP(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec3 maxP(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = 0; j < 3; ++j) {
			minP[i] = fmin(m_data[j][i], minP[i]);
			maxP[i] = fmax(m_data[j][i], maxP[i]);
		}

		// prevent axis-aligned triangle
		if (minP[i] + EPSILON >= maxP[i]) {
			minP[i] = maxP[i] - 0.1f;
			maxP[i] = minP[i] + 0.2f;
		}
	}
	out_box = AABB(minP, maxP);
	return true;
}


std::ostream& KT::operator<<(std::ostream& out, const KT::Triangle& s)
{
	out << " a: " << s.m_a << " b: " << s.m_b << " c: " << s.m_c;
	return out;
}
