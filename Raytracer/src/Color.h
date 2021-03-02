#pragma once
#include <algorithm>
#include <glm\vec3.hpp>
#include <glm\common.hpp>

struct Color {
	glm::dvec3 vec_{};
	constexpr explicit Color(double r, double g, double b) noexcept 
		: vec_{r,g,b} {}
	constexpr explicit Color(const glm::dvec3& vec) noexcept 
		: vec_{vec} {}
	constexpr Color() = default;
	constexpr double r() const noexcept { return vec_.r; }
	constexpr double g() const noexcept { return vec_.g; }
	constexpr double b() const noexcept { return vec_.b; }
	constexpr Color& operator+=(const Color& b) noexcept;
};

constexpr Color convolute(const Color& a, const Color& b) noexcept {
	return Color(a.vec_ * b.vec_);
}

constexpr Color operator+(const Color& a, const Color& b) noexcept {
	return Color( a.vec_ + b.vec_ );
}

constexpr Color& Color::operator+=(const Color& rhs) noexcept {
	this->vec_ += rhs.vec_;
	return *this;
}

constexpr Color operator/(const Color& c, double d) {
	return Color( c.vec_ / d );
}

constexpr Color clamp(const Color& c, double min, double max) {
	return Color(glm::clamp(c.vec_, min, max));
}

static_assert(std::is_constructible_v<Color>, "Color not copy constructible!");
static_assert(std::is_copy_assignable_v<Color>, "Color not copy assignable!");