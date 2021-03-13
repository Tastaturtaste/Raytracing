#pragma once
#include <optional>
#include <array>
#include <glm\vec3.hpp>
#include "Material.h"
#include "IntersectionTrace.h"
#include "Ray.h"
#include "Constants.h"
#include "BoundingBox.h"

class Triangle
{
private:
	Material const* mat_{};
	std::array<glm::dvec3, 3> vertices_;
	norm3 normal_;
public:
	Triangle(std::array<glm::dvec3, 3> vertices, Material const* mat) 
		: mat_{ mat }, vertices_(vertices){
		auto leg1{ vertices_[0] - vertices_[1] };
		auto leg2{ vertices_[0] - vertices_[2] };
		if (glm::dot(leg1,leg1) < constants::EPS)
			leg1 = vertices_[1] - vertices_[2];
		else if (glm::dot(leg2, leg2) < constants::EPS)
			leg2 = vertices_[1] - vertices_[2];
		normal_ = norm3(glm::cross(leg1,leg2));
	}
	std::optional<IntersectionTrace> intersect(const Ray&) const;
	norm3 normal([[maybe_unused]] const glm::dvec3& hitPos) const noexcept {
		return normal_;
	}
	constexpr BoundingBox boundingBox() const {
		BoundingBox box{ .min = vertices_[0], .max = vertices_[0]};
		for(auto i = 1; i < vertices_.size(); ++i){
			box.min.x = glm::min(box.min.x, vertices_[i].x);
			box.min.y = glm::min(box.min.y, vertices_[i].y);
			box.min.z = glm::min(box.min.z, vertices_[i].z);
			box.max.x = glm::max(box.max.x, vertices_[i].x);
			box.max.y = glm::max(box.max.y, vertices_[i].y);
			box.max.z = glm::max(box.max.z, vertices_[i].z);
		}
		return box;
	}
	constexpr Material const* const material() const noexcept { return mat_; }
	template<std::size_t i>
	constexpr glm::dvec3 vertex() const { return vertices_[i]; }
	constexpr auto const& vertices() const { return vertices_; }
	template<std::size_t i1, std::size_t i2>
	constexpr glm::dvec3 edgeFromTo() const{ return vertices_[i2] - vertices_[i1]; }
};

static_assert(std::is_copy_constructible_v<Triangle>, "Triangle not copy constructible!");
static_assert(std::is_copy_assignable_v<Triangle>, "Triangle not copy assignable!");