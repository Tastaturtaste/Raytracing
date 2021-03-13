#pragma once
#include <optional>
#include <concepts>
#include <glm\vec3.hpp>
#include "Material.h"
#include "IntersectionTrace.h"
#include "Ray.h"
#include "BoundingBox.h"


template<class T>
concept Object = requires(T t, glm::dvec3 vec, Ray ray) {
	{ t.normal(vec) } -> std::convertible_to<norm3>;
	{ t.intersect(ray) } -> std::convertible_to<std::optional<IntersectionTrace>>;
	{ t.boundingBox() } -> std::same_as<BoundingBox>;
	{ t.material() } noexcept -> std::convertible_to<Material const* const>;
};