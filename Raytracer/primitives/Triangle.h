#pragma once
#include "Material.h"
#include "IntersectionTrace.h"
#include <optional>
#include <array>
#include <glm\vec3.hpp>
#include "Ray.h"
#include "BasicGeometry.h"
#include "Constants.h"


class Triangle : public BasicGeometry<Triangle>
{
	friend class BasicGeometry<Triangle>;
	std::array<glm::dvec3, 3> vertices_{};
	norm3 normal_{ norm3::xAxis() };

public:
	Triangle(std::array<glm::dvec3, 3> vertices, Material const* mat) 
		: BasicGeometry{ mat }, vertices_(vertices){
		auto leg1{ vertices_[0] - vertices_[1] };
		auto leg2{ vertices_[0] - vertices_[2] };
		if (glm::dot(leg1,leg1) < constants::EPS)
			leg1 = vertices_[1] - vertices_[2];
		else if (glm::dot(leg2, leg2) < constants::EPS)
			leg2 = vertices_[1] - vertices_[2];
		normal_ = norm3(glm::cross(leg1,leg2));
	}
	constexpr Triangle(std::array<glm::dvec3, 3> vertices, Material const* mat, norm3 normal) noexcept
		: BasicGeometry{ mat }, vertices_(vertices), normal_(normal){}

	norm3 normal([[maybe_unused]] const glm::dvec3& hitPos) const noexcept {
		return normal_;
	}
	template<std::size_t i1, std::size_t i2>
	constexpr glm::dvec3 edgeFromTo() const{ return vertices_[i2] - vertices_[i1]; }
	template<std::size_t i>
	constexpr glm::dvec3 vert() const { return vertices_[i]; }


	std::optional<IntersectionTrace> intersect_impl(const Ray&) const noexcept;
};

static_assert(std::is_copy_constructible_v<Triangle>, "Triangle not copy constructible!");
static_assert(std::is_copy_assignable_v<Triangle>, "Triangle not copy assignable!");
static_assert(std::is_base_of_v<BasicGeometry<Triangle>, Triangle>, "Triangle is not a BasicGeometry!");