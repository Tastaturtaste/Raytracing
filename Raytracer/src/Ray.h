#pragma once
#include "Vec3.h"
#include "Norm3.h"
#include <utility>

class Ray
{
	Vec3 origin_;
	Norm3 direction_;

public:

    constexpr Ray(const Vec3& origin, const Norm3& direction) noexcept
        : origin_(origin), direction_(direction) {}
    constexpr explicit Ray() noexcept
        : origin_(Vec3()), direction_(Norm3::zAxis()) {}

    [[nodiscard]] static constexpr Ray fromTo(const Vec3& point1, const Vec3& point2) {
        return Ray(point1, (point2 - point1).norm());
    }

    [[nodiscard]] constexpr const Vec3& origin() const noexcept { return origin_; }
    [[nodiscard]] constexpr const Norm3& direction() const noexcept { return direction_; }
    [[nodiscard]] constexpr Vec3 positionAlong(double distance) const noexcept { return origin_ + direction_ * distance; }
};