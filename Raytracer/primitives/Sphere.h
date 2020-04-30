#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "Primitive.h"

class Sphere : public Primitive
{
	const glm::dvec3 origin_{};
	const double radius_{};
public:
	constexpr Sphere(const glm::dvec3& origin, double radius, const Material& mat)
		: Primitive{ mat }, origin_(origin), radius_(radius) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const { return radius_; }
	inline norm3 normal(const glm::dvec3& hitPos) const noexcept override { 
		return norm3::knownNormal((hitPos - origin_) / radius_); 
	}

	std::optional<IntersectionTrace> intersect(const Ray& r) const override;
};

