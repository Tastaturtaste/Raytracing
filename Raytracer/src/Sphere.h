#pragma once
#include "Material.h"
#include "Vec3.h"

class Sphere
{
	Vec3 origin_{};
	double radius_{};
	Material mat_;
public:
	constexpr Sphere(Vec3 origin, double radius, Material mat) noexcept
		: origin_(origin), radius_(radius), mat_(mat) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const noexcept { return radius_; }
	constexpr auto material() const noexcept { return mat_; }
	auto getNormal(Vec3 hitPos) const noexcept { return Norm3::knownNormal((hitPos - origin_) / radius_); }
};

