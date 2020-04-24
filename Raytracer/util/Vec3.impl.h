#pragma once
#include "Norm3.h"

inline Norm3 Vec3::norm() const noexcept { return Norm3(*this / len()); }

constexpr Vec3::Vec3(const Norm3& n) noexcept
    : x_(n.x()), y_(n.y()), z_(n.z()) {}

constexpr double Vec3::dot(const Vec3& v) const noexcept { return x_ * v.x() + y_ * v.y() + z_ * v.z(); }
constexpr double Vec3::dot(const Norm3& v) const noexcept { return x_ * v.x() + y_ * v.y() + z_ * v.z(); }
constexpr Vec3 Vec3::cross(const Vec3& v) const noexcept {
    const auto x = y_ * v.z() - z_ * v.y();
    const auto y = z_ * v.x() - x_ * v.z();
    const auto z = x_ * v.y() - y_ * v.x();
    return Vec3(x, y, z);
}
constexpr Vec3 Vec3::cross(const Norm3& v) const noexcept {

    return cross(Vec3(v));
}