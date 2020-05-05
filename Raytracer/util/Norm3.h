#pragma once
#include "Constants.h"
#include <cmath>
#include <array>
#include <cassert>
#include <glm\glm.hpp>
//#include <glm\gtx\rotate_vector.hpp>

class Sphere;

class norm3
{
	glm::dvec3 vec_;

	constexpr explicit norm3(double x, double y, double z)
		: vec_{ x,y,z } {};
	constexpr operator glm::dvec3() const { return glm::dvec3(vec_); }

	constexpr static norm3 knownNormal(const glm::dvec3& vec) { return norm3(vec.x,vec.y,vec.z); }
	friend Sphere;
public:
	explicit norm3(const glm::dvec3& v)
		: vec_{ glm::normalize(v) } {};

	[[nodiscard]] constexpr glm::dvec3 getVec() const { return vec_; }

	[[nodiscard]] constexpr norm3 operator-() const { 
		return knownNormal(-vec_);
	}

	[[nodiscard]] constexpr glm::dvec3 operator+(const norm3& rhs) const noexcept { return vec_ + rhs.vec_; }
	[[nodiscard]] constexpr glm::dvec3 operator+(const glm::dvec3& rhs) const noexcept { return vec_ + rhs; }

	[[nodiscard]] constexpr double x() const noexcept { return vec_.x; }
	[[nodiscard]] constexpr double y() const noexcept { return vec_.y; }
	[[nodiscard]] constexpr double z() const noexcept { return vec_.z; }

	[[nodiscard]] static constexpr norm3 xAxis() { return norm3( 1.0, 0.0, 0.0); }
	[[nodiscard]] static constexpr norm3 yAxis() { return norm3( 0.0, 1.0, 0.0); }
	[[nodiscard]] static constexpr norm3 zAxis() { return norm3( 0.0, 0.0, 1.0 ); }

	inline static std::array<norm3,3> orthogonalFromZ(const norm3& z) {
		const auto x = (std::abs(glm::dot(norm3::xAxis().vec_, z.vec_)) < 1.0 - constants::EPS ? norm3::xAxis() : norm3::yAxis()).vec_;
		const auto y = glm::cross(z.vec_, x);
		const auto xx = glm::normalize(glm::cross(y,z.vec_));
		const auto yy = glm::normalize(y);
		const double dotxz = glm::dot(xx, z.vec_);
		const double dotyz = glm::dot(yy, z.vec_);
		const double dotxy = glm::dot(xx, yy);
		assert(std::abs(glm::dot(xx, z.vec_)) < constants::EPS);
		assert(std::abs(glm::dot(yy, z.vec_)) < constants::EPS);
		assert(std::abs(glm::dot(xx, yy)) < constants::EPS);	// put in unit tests
		return { knownNormal(xx),knownNormal(yy),knownNormal(z.vec_) };
	}
	inline static std::array<norm3, 3> orthogonalFromYZ(const norm3& y, const norm3& z) {
		const auto x = glm::normalize(glm::cross(y.vec_, z.vec_));
		const auto zz = glm::normalize(glm::cross(x, y.vec_));
		const double dotxz = glm::dot(x, z.vec_);
		const double dotyz = glm::dot(y.vec_, zz);
		const double dotxy = glm::dot(x, y.vec_);
		assert(std::abs(glm::dot(x, zz)) < constants::EPS);
		assert(std::abs(glm::dot(y.vec_, zz)) < constants::EPS);
		assert(std::abs(glm::dot(x, y.vec_)) < constants::EPS);
		return { knownNormal(x),knownNormal(y),knownNormal(zz) };
	}

	inline static std::array<norm3, 3> orthogonalFromZY(const norm3& z, const norm3& y) {
		const auto x = glm::normalize(glm::cross(y.vec_, z.vec_));
		const auto yy = glm::normalize(glm::cross(z.vec_, x));
		const double dotxz = glm::dot(x, z.vec_);
		const double dotyz = glm::dot(yy, z.vec_);
		const double dotxy = glm::dot(x, yy);
		assert(std::abs(glm::dot(x, z.vec_)) < constants::EPS);
		assert(std::abs(glm::dot(yy, z.vec_)) < constants::EPS);
		assert(std::abs(glm::dot(x, yy)) < constants::EPS);
		return { knownNormal(x),knownNormal(yy),knownNormal(z) };
	}
};


