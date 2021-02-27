#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"


template<class Derived>
class BasicGeometry {
public:
	constexpr Material const* material() const noexcept { return &mat_; }
	norm3 normal(const glm::dvec3& hitPos) const noexcept { return get_impl().normal(hitPos); }
	std::optional<IntersectionTrace> intersect(const Ray& ray) const noexcept { return get_impl().intersect_impl(ray); }
protected:
	constexpr BasicGeometry(Material mat) noexcept : mat_(std::move(mat)) {}
	Material mat_{};
private:
	constexpr Derived const& get_impl() const noexcept {
		return static_cast<Derived const&>(*this);
	}
};