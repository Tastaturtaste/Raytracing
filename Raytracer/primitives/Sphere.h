#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "BasicGeometry.h"


class Sphere : public BasicGeometry<Sphere>
{
	friend class BasicGeometry<Sphere>;
	glm::dvec3 origin_{};
	double radius_{};
public:
	constexpr Sphere(const glm::dvec3& origin, double radius, const Material& mat)
		: BasicGeometry{ mat }, origin_(origin), radius_(radius) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const { return radius_; }
	norm3 normal(const glm::dvec3& hitPos) const noexcept { 
		return norm3::knownNormal((hitPos - origin_) / radius_); 
	}
private:
	std::optional<IntersectionTrace> intersect_impl(const Ray& r) const noexcept;
};

static_assert(std::is_copy_constructible_v<Sphere>, "Sphere not copy constructible!");
static_assert(std::is_copy_assignable_v<Sphere>, "Sphere not copy assignable!");