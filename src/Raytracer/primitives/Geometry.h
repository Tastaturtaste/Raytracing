#pragma once
#include <variant>
#include <concepts>

#include <concepts>
#include "BoundingBox.h"
#include "GeometryConcept.h"
#include "Triangle.h"
#include "Sphere.h"

template<Object... T>
using GeometryVariant = std::variant<T...>;

using GeometryVariant_t = GeometryVariant<Sphere, Triangle>;


// Type erased interface for geometry objects
class Geometry
{
public:
	// Restrict such that this cannot accidentally be mistaken for the copy-constructor
	template<class T>
	requires !std::derived_from<std::decay_t<T>, Geometry> && std::convertible_to<T,GeometryVariant_t>
	constexpr Geometry(T&& geo) : storage(std::forward<T>(geo)) {}

	Material const* material() const noexcept { 
		return std::visit([](auto const& geo) noexcept { return geo.material(); }, storage); 
	}
	norm3 normal(const glm::dvec3& hitPos) const {
		return std::visit([&hitPos](auto const& geo) { return geo.normal(hitPos); }, storage);
	}
	std::optional<IntersectionTrace> intersect(const Ray& ray) const {
		return std::visit([&ray](auto const& geo) { return geo.intersect(ray); }, storage);
	}
	BoundingBox boundingBox() const {
		return std::visit([](auto const& geo) { return geo.boundingBox(); }, storage);
	}
private:
	GeometryVariant_t storage;
};

static_assert(std::is_trivially_copy_constructible_v<Geometry>);
static_assert(std::is_trivially_copyable_v<Geometry>);