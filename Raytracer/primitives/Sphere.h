#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "Primitive.h"

class Sphere : public Primitive
{
	glm::dvec3 origin_{};
	double radius_{};
public:
	constexpr Sphere(glm::dvec3 origin, double radius, Material mat)
		: Primitive{ mat }, origin_(origin), radius_(radius) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const { return radius_; }
	norm3 normal(glm::dvec3 hitPos) const override { 
		return norm3(hitPos - origin_); 
	}

	std::optional<IntersectionTrace> intersect(const Ray& r) const override;
};

