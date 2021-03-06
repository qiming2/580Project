#include <vector>
#include <iostream>
#include "580math.hpp"
#include "Camera.hpp"
#include "Material.h"

namespace KT {
	class Record;
	class Surface;
	class AABB {
	public:
		AABB() {}
		AABB(const KT::vec3& a, const KT::vec3& b) : minP(a), maxP(b) {}
		bool hit(const KT::ray& r) const;

		KT::vec3 minP;
		KT::vec3 maxP;
	};



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
		virtual bool bbox(AABB& out_box) const { return {}; };
	};

	class Sphere : public Surface {
	public:
		Sphere() {};
		Sphere(const vec3& o, const float& r, std::shared_ptr<material> mat) : m_o(o), m_r(r), mat_ptr(mat) {}

		std::shared_ptr<material> mat_ptr;
		virtual Record intersection(const ray& r) const override;
		virtual bool bbox(AABB& out_box) const override;
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

		union {
			struct {
				vec3 m_a_uv;
				vec3 m_b_uv;
				vec3 m_c_uv;
			};
			struct {
				vec3 v0_uv;
				vec3 v1_uv;
				vec3 v2_uv;
			};
			vec3 m_uv_data[3];
		};
		std::shared_ptr<material> mat_ptr;
		std::shared_ptr<texture>* normap;
		Triangle() {};
		Triangle(const vec3& a, const vec3& b, const vec3& c) : m_a(a), m_b(b), m_c(c) {}
		virtual Record intersection(const ray& r) const override;
		virtual bool bbox(AABB& out_box) const override;
		friend std::ostream& operator<<(std::ostream& out, const Triangle& s);
	};
	std::ostream& operator<<(std::ostream& out, const Triangle& s);
	// Surface Manager: Contains all necessary object data necessary for doing
	// intersection test
	// Singleton class
	class BVH_Node : public Surface {
	public:
		BVH_Node() {};
		BVH_Node(std::vector<std::shared_ptr<Surface>> lists, size_t start, size_t end);
		virtual Record intersection(const ray& r) const override;
		virtual bool bbox(AABB& out_box) const override;
	public:
		std::shared_ptr<Surface> left;
		std::shared_ptr<Surface> right;
		AABB box;
	};
	class SurfaceManager {

	public:

		static SurfaceManager& getInstance();
		void Add(std::shared_ptr<Surface> surf);
		void make_random_scene();
		Record intersection(const ray& r, size_t level, size_t max_level, const Camera& c);
		void operator=(const SurfaceManager&) = delete;
		SurfaceManager(const SurfaceManager&) = delete;
		friend std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);
		void construct_BVH();
		void set_env_map(const std::string& dir);
		vec3 getEnvColor(const vec3& dir);
		virtual bool bbox(AABB& out_box);
	public:
		bool have_background;
		std::vector<std::shared_ptr<Surface>> surfaces;
	private:
		// static SurfaceManager* instance;
		BVH_Node cur;
		std::shared_ptr<cube_map> env_map;
		float castShadow(const vec3& point, const vec3& lightDir) const;
		SurfaceManager();
	};
	std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);



	inline bool box_compare(const std::shared_ptr<Surface> a, const std::shared_ptr<Surface> b, int axis) {
		AABB ba;
		AABB bb;

		if (!a->bbox(ba) || !b->bbox(bb)) {
			std::cerr << "NO BOUNDING BOX IN BVH NODE" << std::endl;
		}

		return ba.minP[axis] < bb.minP[axis];
	}

	inline bool box_x_compare(const std::shared_ptr<Surface> a, const std::shared_ptr<Surface> b) {
		return box_compare(a, b, 0);
	}

	inline bool box_y_compare(const std::shared_ptr<Surface> a, const std::shared_ptr<Surface> b) {
		return box_compare(a, b, 1);
	}

	inline bool box_z_compare(const std::shared_ptr<Surface> a, const std::shared_ptr<Surface> b) {
		return box_compare(a, b, 2);
	}

	AABB enclose_box(const AABB& b0, const AABB& b1);
}
