#include "BVH.h"

bool AABB::hit(const KT::ray& r)
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
		t_max = fmin(t0, t_min);
		if (t_max >= t_min) return false;
	}

	return true;
}
