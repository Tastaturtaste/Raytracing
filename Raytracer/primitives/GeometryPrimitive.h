#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"
#include <concepts>


template<class Derived>
class GeometryPrimitive {
public:
	constexpr const Material& material() const noexcept { return mat_; }
	norm3 normal(const glm::dvec3& hitPos) const noexcept { get_impl().normal(hitPos); }
	std::optional<IntersectionTrace> intersect(const Ray& ray) const noexcept { get_impl().intersect(ray); }
protected:
	constexpr GeometryPrimitive(Material mat) noexcept
		: mat_(mat) {}
	Material mat_{};
private:
	constexpr Derived& get_impl() const noexcept {
		return *static_cast<Derived&>(*this);
	}
};