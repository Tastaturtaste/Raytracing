#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include "Vec3.h"
#include "Ray.h"

class Primitive {
	Material mat_;

protected:
	constexpr Primitive(Material mat) noexcept
		: mat_(mat) {}

public:
	virtual ~Primitive() = default;
	
	constexpr Material material() const noexcept { return mat_; }
	virtual Norm3 normal(Vec3 hitPos) const noexcept = 0;
	virtual std::optional<IntersectionTrace> intersect(const Ray&) const = 0;
};