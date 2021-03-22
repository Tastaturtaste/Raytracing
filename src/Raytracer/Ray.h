#pragma once
#include <glm\vec3.hpp>
#include "norm3.h"
#include <utility>

class Ray
{
	glm::dvec3 origin_;
	norm3 direction_;

public:

    constexpr Ray(glm::dvec3 origin, norm3 direction) noexcept
        : origin_(std::move(origin)), direction_(std::move(direction)) {}
    constexpr explicit Ray() noexcept
        : origin_(0.0,0.0,0.0), direction_(norm3::zAxis()) {}

    [[nodiscard]] static constexpr Ray fromTo(const glm::dvec3& point1, const glm::dvec3& point2) {
        return Ray(point1, norm3(point2 - point1));
    }

    [[nodiscard]] constexpr const glm::dvec3& origin() const noexcept { return origin_; }
    [[nodiscard]] constexpr const norm3& direction() const noexcept { return direction_; }
    [[nodiscard]] constexpr glm::dvec3 positionAlong(double distance) const noexcept { return origin_ + distance * direction_.getVec(); }
};