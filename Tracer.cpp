#include "Tracer.hpp"
#include "util.hpp"

KT::Record::Record(Surface* surf, const vec3& norm, float t) :m_surf(surf), m_normal(norm), m_t(t)
{

}

KT::Record::Record():m_t(std::numeric_limits<float>::infinity())
{

}

KT::Record::Record(Surface* surf, float t) : m_surf(surf), m_t(t)
{

}

KT::Record KT::Sphere::intersection(const ray& r) const
{
	Record record;
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

		record.m_normal = point - m_o;
		record.m_normal.normalize();
		//println(record.m_normal);
		return record;
	}

	float t2 = (-e_dot_d + discrim) / d_dot_d;
	if (t2 > HIT_EPSILON) {
		record.m_surf = this;
		record.m_t = t2;

		point = r.eval(t2);

		record.m_normal = point - m_o;
		record.m_normal.normalize();
		return record;
	}

	return record;
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

void KT::SurfaceManager::Add(Surface& surf)
{
	surfaces.push_back(&surf);
}

extern KT::vec3 globalDir;
KT::Record KT::SurfaceManager::intersection(const ray& r, size_t level, size_t max_level, const Camera& c)
{
	Record ret;
	if (level == max_level) return ret;


	Record record;

	for (size_t i = 0; i < surfaces.size(); ++i) {
		record = surfaces[i]->intersection(r);
		if (record.m_surf && ret.m_t > record.m_t - EPSILON) {
			ret = record;
		}
	}
	// Add a directional light
	//const static vec3 color = vec3(rand() % 100 / 100.0f / 2.0f + 0.5f, 0.7f, rand() % 100 / 100.0f);
	const static vec3 color = vec3(1.0f, 0.64f, 0.84f);
	static bool printOnceColor = false;
	if (!printOnceColor) {
		print("Random color: ", color);
		printOnceColor = true;
	}

	//const static vec3 lightDir = normalize(vec3(-0.5f, 0.3f, -0.5f));
	const static vec3 lightDir = normalize(globalDir);
	const static vec3 lightColor = vec3(0.7f, 0.7f, 0.7f);
	const static vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
	const static float shinness = 32.0f;
	vec3 output_color;
	vec3 hitPoint;
	vec3 halfDir;
	float dotH;
	float dotHN;
	float shadow;
	if (ret.m_surf) {
		// 3. Shading (flat shading without light first -> phong shading -> pbr)
		hitPoint = r.eval(ret.m_t);

		// Shadow Casting

		shadow = castShadow(hitPoint, lightDir);
		dotH = std::max(ret.m_normal.dot(-lightDir), 0.0f);

		// phong shading
		// ambient + diffuse + specular
		(halfDir = (-lightDir) + (c.m_frame.getPos() - normalize(hitPoint))).normalize();
		dotHN = std::max(halfDir.dot(ret.m_normal), 0.0f);
		output_color = ambientColor * color;
		output_color += (dotH * color * lightColor + pow(dotHN, shinness) * lightColor * color) * (1.0f - shadow);

		ret.m_color = output_color;

		// If hit something, we do a reflection
		ray reflect_ray;
		reflect_ray.m_o = hitPoint;
		reflect_ray.m_d = r.m_d + 2 * (-r.m_d.dot(ret.m_normal)) * ret.m_normal;
		// TODO Store some sort of material
		ret.m_color += 0.3 * intersection(reflect_ray, level + 1, max_level, c).m_color;
	}
	return ret;
}

// TODO: Change lightDir to maybe light info
float KT::SurfaceManager::castShadow(const vec3& point, const vec3& lightDir) const
{
	const ray shadowRay(point, normalize(-lightDir));
	for (size_t i = 0; i < surfaces.size(); ++i) {
		if (surfaces[i]->intersection(shadowRay).m_surf) {
			//print("enter?");
			return 1.0f;
		}
	}
	return 0.0f;
}

KT::SurfaceManager::SurfaceManager()
{

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
	if (determ < EPSILON) return ret;
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
	ret.m_normal = v0v1.cross(v0v2);
	ret.m_normal.normalize();


	return ret;
}


std::ostream& KT::operator<<(std::ostream& out, const KT::Triangle& s)
{
	out << " a: " << s.m_a << " b: " << s.m_b << " c: " << s.m_c;
	return out;
}
