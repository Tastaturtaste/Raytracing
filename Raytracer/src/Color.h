#pragma once
#include <algorithm>
#include <glm\vec3.hpp>
#include <glm\common.hpp>

struct Color {
	constexpr explicit Color(double r, double g, double b) noexcept 
		: vec_{r,g,b} {}
	constexpr explicit Color(const glm::dvec3& vec) noexcept 
		: vec_{vec} {}
	constexpr Color() = default;
	glm::dvec3 vec_{};
/*	using r = vec_.r;
	using g = vec_.g;
	using b = vec_.b;*/
	constexpr double r() const noexcept { return vec_.r; }
	constexpr double g() const noexcept { return vec_.g; }
	constexpr double b() const noexcept { return vec_.b; }
	constexpr Color& operator+=(const Color& b) noexcept;
};

constexpr Color convolute(const Color& a, const Color& b) noexcept {
	return  Color(a.r() * b.r(),a.g() * b.g(),a.b() * b.b() );
}

constexpr Color operator+(const Color& a, const Color& b) noexcept {
	return Color( a.vec_ + b.vec_ );
}

constexpr Color& Color::operator+=(const Color& rhs) noexcept {
	this->vec_ += rhs.vec_;
	return *this;
}

constexpr Color operator/(const Color& c, double d) {
	return Color( c.vec_ * (1.0f / d) );
}

constexpr Color clamp(const Color& c, double min, double max) {
	return Color(glm::clamp(c.vec_, min, max));
}