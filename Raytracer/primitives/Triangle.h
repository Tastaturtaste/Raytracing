#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <array>
#include "Vec3.h"
#include "Ray.h"
#include "Primitive.h"
#include "Constants.h"


class Triangle : public Primitive
{
	std::array<Vec3, 3> vertices_{};
	Norm3 normal_{ Norm3::xAxis() };

public:
	Triangle(std::array<Vec3, 3> vertices, Material mat) noexcept
		: Primitive{ mat }, vertices_(vertices){
		auto leg1{ vertices_[0] - vertices_[1] };
		auto leg2{ vertices_[0] - vertices_[2] };
		if (leg1.lensq() < constants::EPS)
			leg1 = vertices_[1] - vertices_[2];
		else if (leg2.lensq() < constants::EPS)
			leg2 = vertices_[1] - vertices_[2];
		normal_ = leg1.cross(leg2).norm();
	}
	constexpr Triangle(std::array<Vec3, 3> vertices, Material mat, Norm3 normal) noexcept
		: Primitive{ mat }, vertices_(vertices), normal_(normal){}

	Norm3 normal([[maybe_unused]] Vec3 hitPos) const noexcept override {
		return normal_;
	}
	template<std::size_t i1, std::size_t i2>
	constexpr Vec3 leg() const{ return vertices_[i2] - vertices_[i1]; }
	template<std::size_t i>
	constexpr Vec3 vert() const { return vertices_[i]; }


	std::optional<IntersectionTrace> intersect(const Ray&) const override;
};