#pragma once
#include <algorithm>

struct Color {
	double r;
	double g;
	double b;

	constexpr Color& operator+=(const Color& b) noexcept;
};

constexpr Color convolute(const Color& a, const Color& b) noexcept {
	return { .r{a.r * b.r},.g{a.g * b.g},.b{a.b * b.b} };
}

constexpr Color operator+(const Color& a, const Color& b) noexcept {
	return { .r{a.r + b.r},.g{a.g + b.g},.b{a.b + b.b} };
}

constexpr Color& Color::operator+=(const Color& rhs) noexcept {
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}

constexpr Color operator/(const Color& c, double d) {
	return { .r{c.r / d},.g{c.g / d},.b{c.b / d} };
}

constexpr Color clamp(const Color& c, double min, double max) {
	return { .r{std::clamp(c.r,min,max)},.g{std::clamp(c.g,min,max)},.b{std::clamp(c.b,min,max)} };
}