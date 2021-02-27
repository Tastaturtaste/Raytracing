#pragma once
#include <variant>
#include <concepts>
#include "BasicGeometry.h"
#include "Triangle.h"
#include "Sphere.h"

using GeometryVariant_t = std::variant<Sphere, Triangle>;

// Type erased interface for geometry objects
class Geometry
{
public:
	// Restrict such that this cannot accidentally be mistaken for the copy-constructor
	template<class T>
	requires !std::derived_from<std::decay_t<T>, Geometry>
	constexpr Geometry(T&& geo) : storage(std::forward<T>(geo)) {}

	Material const* material() const noexcept { 
		return std::visit([](auto const& geo) noexcept { return geo.material(); }, storage); 
	}
	norm3 normal(const glm::dvec3& hitPos) const noexcept {
		return std::visit([&hitPos](auto const& geo) noexcept { return geo.normal(hitPos); }, storage);
	}
	std::optional<IntersectionTrace> intersect(const Ray& ray) const noexcept {
		return std::visit([&ray](auto const& geo) noexcept { return geo.intersect(ray); }, storage);
	}
private:
	GeometryVariant_t storage;
};

