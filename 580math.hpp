#pragma once
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <memory>
#include "util.hpp"
#define EPSILON 0.0001f
#define SINE_EPSILON 0.2f
#define HIT_EPSILON 0.01f

#define DTOR(x) ((x) / 180.0f * M_PI)

namespace KT {
	inline double random_double() {
		// returns a random real in [0, 1)
		return rand() / (RAND_MAX + 1.0);
	}

	inline double random_double(double dmin, double dmax) {
		// returns a random real in [dmin, dmax]
		return dmin + (dmax - dmin) * random_double();
	}
	struct vec3
	{
		union {
			struct {
				float m_x;
				float m_y;
				float m_z;
			};
			float data[3];
		};
		vec3(float v): m_x(v), m_y(v), m_z(v) {}
		vec3():m_x(0.0f), m_y(0.0f), m_z(0.0f) {}
		vec3(float x, float y, float z):
			m_x(x), m_y(y), m_z(z)
		{
		}

		inline float& operator[](const size_t& index) {
			return data[index];
		}

		inline const float& operator[](const size_t& index) const {
			return data[index];
		}

		float dot(const vec3& other) const {
			return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;
		}

		vec3 cross(const vec3& other) const {
			return { m_y * other.m_z - other.m_y * m_z, m_z * other.m_x - m_x * other.m_z, m_x * other.m_y - m_y * other.m_x };
		}

		void normalize() {
			float length = len();
			m_x /= length;
			m_y /= length;
			m_z /= length;
		}

		float len() const {
			return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z);
		}

		float len_squared() const {
			return m_x * m_x + m_y * m_y + m_z * m_z;
		}

		vec3 operator-() const {
			return vec3(-m_x, -m_y, -m_z);
		}

		void operator=(const vec3& v) {
			m_x = v.m_x;
			m_y = v.m_y;
			m_z = v.m_z;
		}

		vec3 operator*(const vec3& other) const {
			return vec3(m_x * other.m_x, m_y * other.m_y, m_z * other.m_z);
		}

		vec3 operator+(const vec3& other) const {
			return vec3(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
		}

		void operator+=(const vec3& other) {
			m_x += other.m_x;
			m_y += other.m_y;
			m_z += other.m_z;
		}

		vec3 operator-(const vec3& other) const {
			return vec3(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
		}

		vec3 operator*(float s) const {
			return vec3(m_x * s, m_y * s, m_z * s);
		}

		vec3 operator/(float s) const {
			return vec3(m_x / s, m_y / s, m_z / s);
		}

		bool near_zero() {
			const static float ep = 1e-8;
			return (fabs(m_x) < ep) && (fabs(m_y) < ep) && (fabs(m_z) < ep);
		}

		inline static vec3 random() {
			return vec3(random_double(), random_double(), random_double());
		}

		inline static vec3 random(double dmin, double dmax) {
			return vec3(random_double(dmin, dmax), random_double(dmin, dmax), random_double(dmin, dmax));
		}

		// rejection method
		inline static vec3 random_unit_sphere() {
			while (true) {
				vec3 generated = random(-1.0, 1.0);
				if (generated.len_squared() > 1.0f - EPSILON) {
					continue;
				}
				return generated;
			}
		}

		// rejection method
		inline static vec3 random_unit_disk() {
			while (true) {
				vec3 generated = vec3(random_double(-1.0f, 1.0f), random_double(-1.0f, 1.0f), 0.0f);
				if (generated.len_squared() > 1.0f - EPSILON) {
					continue;
				}
				return generated;
			}
		}

		inline static vec3 random_unit_vec3() {
			vec3 ret = random_unit_sphere();
			ret.normalize();
			return ret;
		}

		friend vec3 normalize(const vec3& v) {
			vec3 ret = v;
			ret.normalize();
			return ret;
		}

		friend vec3 operator*(float s, const vec3& other) {
			return vec3(other.m_x * s, other.m_y * s, other.m_z * s);
		}

		friend std::ostream& operator<<(std::ostream& out, const vec3& data) {
			out << data.m_x << " " << data.m_y << " " << data.m_z;
			return out;
		}
	};
	inline float dot(const vec3& v1, const vec3& v2) {
		return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
	}
	inline vec3 reflect(const vec3& v1, const vec3& n) {
		return v1 - 2 * dot(v1, n) * n;
	}

	inline vec3 refract(vec3 v, vec3 n, const float& indexin_over_indexout) {
		float cos_theta = fmin(dot(-v, n), 1.0f);
		vec3 out_perp = indexin_over_indexout * (v + cos_theta * n);
		vec3 out_parp = -sqrt(fabs(1.0 - out_perp.len_squared())) * n;
		return out_perp + out_parp;
	}

	struct vec4
	{
		union {
			struct {
				float m_x;
				float m_y;
				float m_z;
				float m_w;
			};
			float data[4];
		};
		vec4() :m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f) {}
		vec4(float x, float y, float z, float w) :
			m_x(x), m_y(y), m_z(z), m_w(w)
		{
		}

		float dot(const vec4& other) const {
			return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z + m_w * other.m_w;
		}

		void normalize() {
			float length = len();
			m_x /= length;
			m_y /= length;
			m_z /= length;
			m_w /= length;
		}

		friend vec4 normalize(const vec4& v) {
			vec4 ret = v;
			ret.normalize();
			return ret;
		}

		float len() const {
			return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
		}

		vec4 operator-() const {
			return vec4(-m_x, -m_y, -m_z, -m_w);
		}

		vec4 operator*(const vec4& other) const {
			return vec4(m_x * other.m_x, m_y * other.m_y, m_z * other.m_z, m_w * other.m_w);
		}

		vec4 operator+(const vec4& other) const {
			return vec4(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z, m_w + other.m_w);
		}

		void operator+=(const vec4& other) {
			m_x += other.m_x;
			m_y += other.m_y;
			m_z += other.m_z;
			m_w += other.m_w;
		}

		vec4 operator-(const vec4& other) const {
			return vec4(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z, m_w * other.m_w);
		}

		vec4 operator*(float s) const {
			return vec4(m_x * s, m_y * s, m_z * s, m_w * s);
		}
		friend vec4 operator*(float s, const vec4& other) {
			return vec4(other.m_x * s, other.m_y * s, other.m_z * s, other.m_w * s);
		}
		friend std::ostream& operator<<(std::ostream& out, const vec4& data) {
			out << data.m_x << " " << data.m_y << " " << data.m_z << " " << data.m_w;
			return out;
		}
	};


	struct ray {
		vec3 m_o; // origin
		vec3 m_d; // direction
		double time;
		ray() {}
		ray(vec3 o, vec3 d, double t = 0.0):
			m_o(o), m_d(d), time(t)
		{

		}

		vec3 eval(float t) const {
			return m_o + m_d * t;
		}

		friend std::ostream& operator<<(std::ostream& out, const ray& r) {
			out << "Origin: " << r.m_o << " ";
			out << "Direction: " << r.m_d;
			return out;
		}
	};

	struct mat4 {
		union {
			float m[4][4];
			float m16[16];
		};

		mat4() {
			clear();
			identity();
		}

		mat4(const float& m0, const float& m1, const float& m2, const float& m3,
			 const float& m4, const float& m5, const float& m6, const float& m7,
			 const float& m8, const float& m9, const float& m10, const float& m11,
			 const float& m12,const float& m13, const float& m14, const float& m15) {
			m[0][0] = m0; m[0][1] = m4; m[0][2] = m8; m[0][3] = m12;
			m[1][0] = m1; m[0][1] = m5; m[0][2] = m9; m[0][3] = m13;
			m[2][0] = m2; m[0][1] = m6; m[0][2] = m10; m[0][3] = m14;
			m[3][0] = m3; m[0][1] = m7; m[0][2] = m11; m[0][3] = m15;
		}


		mat4(const vec3& u, const vec3& v, const vec3& w, const vec3& pos) {
			setU(u); setV(v); setW(w); setPos(pos);
		}

		void setU(const vec3& u) {
			m[0][0] = u.m_x;
			m[1][0] = u.m_y;
			m[2][0] = u.m_z;
		}

		vec3 getU() const {
			return vec3(m[0][0], m[1][0], m[2][0]);
		}
		vec3 getV() const {
			return vec3(m[0][1], m[1][1], m[2][1]);
		}
		vec3 getW() const {
			return vec3(m[0][2], m[1][2], m[2][2]);
		}
		vec3 getPos() const {
			return vec3(m[0][3], m[1][3], m[2][3]);
		}
		void setV(const vec3& v) {
			m[0][1] = v.m_x;
			m[1][1] = v.m_y;
			m[2][1] = v.m_z;
		}

		void setW(const vec3& w) {
			m[0][2] = w.m_x;
			m[1][2] = w.m_y;
			m[2][2] = w.m_z;
		}

		void setPos(const vec3& pos) {
			m[0][3] = pos.m_x;
			m[1][3] = pos.m_y;
			m[2][3] = pos.m_z;
		}

		void identity() {
			clear();
			m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		}

		void clear() {
			memset((void*)m, 0, sizeof(m));
		}

		friend std::ostream& operator<<(std::ostream& out, const mat4& m) {
			out << m.m[0][0] << " " << m.m[0][1] << " " << m.m[0][2] << " " << m.m[0][3] << "\n"
				<< m.m[1][0] << " " << m.m[1][1] << " " << m.m[1][2] << " " << m.m[1][3] << "\n"
				<< m.m[2][0] << " " << m.m[2][1] << " " << m.m[2][2] << " " << m.m[2][3] << "\n"
				<< m.m[3][0] << " " << m.m[3][1] << " " << m.m[3][2] << " " << m.m[3][3];
			return out;
		}
	};

}
