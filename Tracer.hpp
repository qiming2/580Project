#include <vector>
#include <iostream>
#include "580math.hpp"
#include "Camera.hpp"
#include "Material.h"

namespace KT {
	class Record;
	class Surface;
	class Record {
	public:
		vec3 m_color;
		vec3 m_normal;
		float u;
		float v;
		const Surface* m_surf = nullptr;
		float m_t = std::numeric_limits<float>::infinity();
		std::shared_ptr<material> mat_ptr;
		bool front_face;
		Record();
		Record(Surface* surf, float t);
		Record(Surface* surf, const vec3& norm, float t);

		// we need to check the face normal since
		// ray might bounce inside a sphere
		inline void set_face_normal(const ray& r, const vec3& outward_normal) {
			front_face = dot(r.m_d, outward_normal) < 0.0f;
			m_normal = front_face ? outward_normal : -outward_normal;
		}
		
		friend std::ostream& operator<<(std::ostream& out, const Record& s);
	};
	std::ostream& operator<<(std::ostream& out, const Record& s);

	class Surface {
	public:
		virtual Record intersection(const ray& r) const { return {}; };
	};

	class Sphere : public Surface {
	public:
		Sphere() {};
		Sphere(const vec3& o, const float& r, std::shared_ptr<material> mat) : m_o(o), m_r(r), mat_ptr(mat) {}
		
		std::shared_ptr<material> mat_ptr;
		virtual Record intersection(const ray& r) const override;
		
		friend std::ostream& operator<<(std::ostream& out, const Sphere& s);
		static void getUV(const vec3& point, float& u, float& v);
	public:
		vec3 m_o; // origin
		float m_r;
	};
	std::ostream& operator<<(std::ostream& out, const Sphere& s);

	class Triangle : public Surface {
	public:
		union {
			struct {
				vec3 m_a;
				vec3 m_b;
				vec3 m_c;
			};
			struct {
				vec3 v0;
				vec3 v1;
				vec3 v2;
			};
			vec3 m_data[3];
		};
		Triangle() {};
		Triangle(const vec3& a, const vec3& b, const vec3& c) : m_a(a), m_b(b), m_c(c) {}
		virtual Record intersection(const ray& r) const override;
		
		friend std::ostream& operator<<(std::ostream& out, const Triangle& s);
	};
	std::ostream& operator<<(std::ostream& out, const Triangle& s);
	// Surface Manager: Contains all necessary object data necessary for doing
	// intersection test
	// Singleton class
	class SurfaceManager {

	public:

		static SurfaceManager& getInstance();
		void Add(std::shared_ptr<Surface> surf);
		void make_random_scene();
		Record intersection(const ray& r, size_t level, size_t max_level, const Camera& c);
		void operator=(const SurfaceManager&) = delete;
		SurfaceManager(const SurfaceManager&) = delete;
		friend std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);
	private:
		// static SurfaceManager* instance;
		std::vector<std::shared_ptr<Surface>> surfaces;
		float castShadow(const vec3& point, const vec3& lightDir) const;
		SurfaceManager();
	};
	std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);
}
