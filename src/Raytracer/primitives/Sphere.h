#pragma once
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "Material.h"
#include "IntersectionTrace.h"
#include "BoundingBox.h"


class Sphere
{
private:
	Material const* mat_;
	glm::dvec3 origin_{};
	double radius_{};
public:
	std::optional<IntersectionTrace> intersect(const Ray& r) const;
	norm3 normal(const glm::dvec3& hitPos) const { 
		return norm3::knownNormal((hitPos - origin_) / radius_); 
	}
	constexpr BoundingBox boundingBox() const {
		return { .min = origin_ - glm::dvec3(1.) * radius_, .max = origin_ + glm::dvec3(1.) * radius_ };
	}
	constexpr Material const* const material() const noexcept { return mat_; }
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const { return radius_; }
	constexpr Sphere(const glm::dvec3& origin, double radius, Material const* mat)
		: mat_{ mat }, origin_(origin), radius_(radius) {}
};

static_assert(std::is_copy_constructible_v<Sphere>, "Sphere not copy constructible!");
static_assert(std::is_copy_assignable_v<Sphere>, "Sphere not copy assignable!");