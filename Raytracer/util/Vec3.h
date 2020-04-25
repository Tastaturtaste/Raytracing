#pragma once
#include <cmath>
#include <cassert>
#include "Constants.h"
#include <array>

class Norm3;

class Vec3
{
	double x_{};
	double y_{};
	double z_{};
public:
	constexpr Vec3() noexcept = default;
	constexpr Vec3(double x, double y, double z) noexcept : x_(x), y_(y), z_(z) {}
	constexpr explicit Vec3(const Norm3& n) noexcept;

	[[nodiscard]] constexpr Vec3 operator+(const Vec3& rhs) const noexcept { return Vec3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_); }
	[[nodiscard]] constexpr Vec3 operator+(double r) const noexcept { return Vec3(x_ + r, y_ + r, z_ + r); }
	[[nodiscard]] constexpr Vec3 operator-(const Vec3& rhs) const noexcept { return Vec3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_); }
	[[nodiscard]] constexpr Vec3 operator-(double r) const noexcept { return Vec3(x_ - r, y_ - r, z_ - r); }


	[[nodiscard]] constexpr double dot(const Vec3& v) const noexcept;
	[[nodiscard]] constexpr double dot(const Norm3& v) const noexcept;
	[[nodiscard]] constexpr Vec3 cross(const Vec3& v) const noexcept;
	[[nodiscard]] constexpr Vec3 cross(const Norm3& v) const noexcept;

	[[nodiscard]] constexpr Vec3 scale(double scl) const noexcept { return Vec3(x_ * scl, y_ * scl, z_ * scl); }
	[[nodiscard]] friend constexpr Vec3 operator*(double lhs, const Vec3& rhs) noexcept { return rhs.scale(lhs); }
	[[nodiscard]] constexpr Vec3 operator*(double rhs) const noexcept { return this->scale(rhs); }

	[[nodiscard]] double len() const noexcept { return sqrt(pow(x_, 2) + pow(y_, 2) + pow(z_, 2)); }
	[[nodiscard]] constexpr double lensq() const noexcept { return x_*x_ + y_*y_ + z_*z_; }
	[[nodiscard]] Norm3 norm() const noexcept;
	[[nodiscard]] constexpr Vec3 operator/(double d) const {
		//assert(fabs(d) < constants::EPS, "division by zero!");
		return Vec3(x_ / d, y_ / d, z_ / d);
	}
	[[nodiscard]] constexpr Vec3 operator-() { return Vec3(-x_, -y_, -z_); }

	[[nodiscard]] constexpr Vec3 transform(const std::array<Norm3, 3> basis);

	[[nodiscard]] constexpr double x() const noexcept { return x_; }
	[[nodiscard]] constexpr double y() const noexcept { return y_; }
	[[nodiscard]] constexpr double z() const noexcept { return z_; }

	[[nodiscard]] static constexpr Vec3 xAxis() { return { 1, 0, 0 }; }
	[[nodiscard]] static constexpr Vec3 yAxis() { return { 0, 1, 0 }; }
	[[nodiscard]] static constexpr Vec3 zAxis() { return { 0, 0, 1 }; }
};

#include "Vec3.impl.h"