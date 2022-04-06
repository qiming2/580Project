#include <vector>
#include <iostream>
#include "580math.hpp"
#include "Camera.hpp"

namespace KT {
	class Record;
	class Surface;
	class Record {
	public:
		vec3 m_color;
		vec3 m_normal;
		const Surface* m_surf = nullptr;
		float m_t = std::numeric_limits<float>::infinity();
		Record();
		Record(Surface* surf, float t);
		Record(Surface* surf, const vec3& norm, float t);
		friend std::ostream& operator<<(std::ostream& out, const Record& s);
	};
	std::ostream& operator<<(std::ostream& out, const Record& s);

	class Surface {
	public:
		virtual Record intersection(const ray& r) const { return {}; };
	};

	class Sphere : public Surface {
	public:
		vec3 m_o; // origin
		float m_r;

		virtual Record intersection(const ray& r) const override;

		friend std::ostream& operator<<(std::ostream& out, const Sphere& s);
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
		void Add(Surface& surf);

		Record intersection(const ray& r, size_t level, size_t max_level, const Camera& c);
		void operator=(const SurfaceManager&) = delete;
		SurfaceManager(const SurfaceManager&) = delete;
		friend std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);
	private:
		// static SurfaceManager* instance;
		std::vector<Surface*> surfaces;
		float castShadow(const vec3& point, const vec3& lightDir) const;
		SurfaceManager();
	};
	std::ostream& operator<<(std::ostream& out, const SurfaceManager& surfman);
}
