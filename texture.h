#pragma once
#include "580math.hpp"


class texture
{
public:
	virtual KT::vec3 getColor(float u, float v, const KT::vec3& hitpoint) const = 0;
};

class solid_color : public texture {
public:
	solid_color(KT::vec3 _color_val);
	virtual KT::vec3 getColor(float u, float v, const KT::vec3& hitpoint) const override;
private:
	KT::vec3 color_val;
};

class checker_texture : public texture {
public:
	checker_texture() {};

	checker_texture(KT::vec3 c1, KT::vec3 c2) :
		even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2)) {}
	virtual KT::vec3 getColor(float u, float v, const KT::vec3& hitpoint) const override;

public:
	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};

class checker_texture2 : public texture {
public:
	checker_texture2() {};

	checker_texture2(KT::vec3 c1, KT::vec3 c2) :
		even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2)) {}
	virtual KT::vec3 getColor(float u, float v, const KT::vec3& hitpoint) const override;

public:
	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};

class water_texture : public texture {
public:
	water_texture() : albedo(KT::vec3(212.0f / 255.0f, 241.0f / 255.0f, 249.0f / 255.0f)) {};

	water_texture(KT::vec3 _albedo) : albedo(_albedo) {}
	virtual KT::vec3 getColor(float u, float v, const KT::vec3& hitpoint) const override;
	

public:
	KT::vec3 albedo;
};