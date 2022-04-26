#pragma once
#include "580math.hpp"

class AABB {
public:
	AABB() {}
	AABB(const KT::vec3& a, const KT::vec3& b): minP(a), maxP(b) {}
	bool hit(const KT::ray& r);

	KT::vec3 minP;
	KT::vec3 maxP;
};

