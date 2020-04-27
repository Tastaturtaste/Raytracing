#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include "Vec3.h"
#include "Ray.h"
#include "Primitive.h"

class Sphere : public Primitive
{
	Vec3 origin_{};
	double radius_{};
	//Material mat_;
public:
	constexpr Sphere(Vec3 origin, double radius, Material mat) noexcept
		: Primitive{ mat }, origin_(origin), radius_(radius) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const noexcept { return radius_; }
	//constexpr auto material() const noexcept { return mat_; }
	Norm3 normal(Vec3 hitPos) const noexcept override { 
		return Norm3::knownNormal((hitPos - origin_) / radius_); 
	}

	std::optional<IntersectionTrace> intersect(const Ray& r) const noexcept override;
};

