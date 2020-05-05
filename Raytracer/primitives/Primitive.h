#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <glm\vec3.hpp>
#include "Ray.h"

class Primitive {
	const Material mat_;

protected:
	constexpr Primitive(Material mat) noexcept
		: mat_(mat) {}

public:
	virtual ~Primitive() = default;
	Primitive() = delete;
	Primitive(const Primitive&) = default;
	Primitive& operator=(const Primitive&) = default;
	Primitive(Primitive&&) = default;
	Primitive& operator=(Primitive&&) = default;
	
	
	constexpr const Material& material() const noexcept { return mat_; }
	virtual norm3 normal(const glm::dvec3& hitPos) const = 0;
	virtual std::optional<IntersectionTrace> intersect(const Ray&) const = 0;
};