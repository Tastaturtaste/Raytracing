#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "GeometryPrimitive.h"


class Sphere : public GeometryPrimitive<Sphere>
{
	friend class GeometryPrimitive<Sphere>;
	const glm::dvec3 origin_{};
	const double radius_{};
public:
	constexpr Sphere(const glm::dvec3& origin, double radius, const Material& mat)
		: GeometryPrimitive{ mat }, origin_(origin), radius_(radius) {}
	constexpr auto origin() const noexcept { return origin_; }
	constexpr auto radius() const { return radius_; }
	norm3 normal(const glm::dvec3& hitPos) const noexcept { 
		return norm3::knownNormal((hitPos - origin_) / radius_); 
	}
private:
	std::optional<IntersectionTrace> intersect_impl(const Ray& r) const noexcept;
};

