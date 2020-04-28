#pragma once
#include <tuple>
#include "Constants.h"
#include <cmath>
#include <array>
#include <cassert>

class Vec3;

class Norm3
{
	double x_{};
	double y_{};
	double z_{};

	friend class Vec3;

	constexpr explicit Norm3(const Vec3& v);
	constexpr explicit Norm3(double x, double y, double z) noexcept
		: x_(x), y_(y), z_(z) {}
public:
	[[nodiscard]] constexpr Vec3 toVec3() const noexcept;
	[[nodiscard]] constexpr Norm3 reflect(const Norm3& incoming) const noexcept;
	[[nodiscard]] constexpr double dot(const Vec3& v) const noexcept;
	[[nodiscard]] constexpr double dot(const Norm3& v) const noexcept;
	[[nodiscard]] constexpr Vec3 cross(const Vec3& v) const noexcept;
	[[nodiscard]] constexpr Vec3 cross(const Norm3& v) const noexcept;

	[[nodiscard]] friend constexpr Vec3 operator*(double lhs, const Norm3& rhs) noexcept;
	[[nodiscard]] constexpr Vec3 operator*(double rhs) const noexcept;

	[[nodiscard]] constexpr Norm3 operator-() { 
		return Norm3(-x_, -y_, -z_); 
	}

	[[nodiscard]] constexpr double x() const noexcept { return x_; }
	[[nodiscard]] constexpr double y() const noexcept { return y_; }
	[[nodiscard]] constexpr double z() const noexcept { return z_; }

	[[nodiscard]] static constexpr Norm3 xAxis() { return Norm3( 1, 0, 0 ); }
	[[nodiscard]] static constexpr Norm3 yAxis() { return Norm3( 0, 1, 0 ); }
	[[nodiscard]] static constexpr Norm3 zAxis() { return Norm3( 0, 0, 1 ); }

	[[nodiscard]] Norm3 rotate(const Norm3& rotationAxis, double alpha) noexcept;

	[[nodiscard]] static std::pair<Norm3, Norm3> orthogonal_from_z(const Norm3& z) noexcept;
	[[nodiscard]] static std::array<Norm3, 3> orthogonalFromYZ(const Norm3& y, const Norm3& z) noexcept;
	[[nodiscard]] static std::array<Norm3, 3> orthogonalFromZY(const Norm3& z, const Norm3& y) noexcept;

	[[nodiscard]] static inline Norm3 knownNormal(double x, double y, double z) noexcept {
		// only use if you are sure the result of the construction is normal
		#ifdef DEBUG
		auto len = x * x + y * y + z * z;
		assert(("Vector is not normal!",fabs(len-1) < constants::EPS))
		#endif
		return Norm3(x, y, z);
	} 
	[[nodiscard]] static inline Norm3 knownNormal(const Vec3& vec) noexcept {
		// only use if you are sure the result of the construction is normal
		#ifdef DEBUG
		auto len = vec.lensq();
		assert(("Vector is not normal!", fabs(len - 1) < constants::EPS))
		#endif
		return Norm3(vec);
	}
};

#include "Norm3.impl.h"

