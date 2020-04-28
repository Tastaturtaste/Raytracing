#pragma once
#include "Vec3.h"

constexpr Norm3::Norm3(const Vec3& v) 
    : x_(v.x()), y_(v.y()), z_(v.z()) {}

constexpr Vec3 Norm3::toVec3() const noexcept { return Vec3(x_, y_, z_); }
constexpr double Norm3::dot(const Vec3& v) const noexcept { return x_ * v.x() + y_ * v.y() + z_ * v.z(); }
constexpr double Norm3::dot(const Norm3& v) const noexcept { return x_ * v.x() + y_ * v.y() + z_ * v.z(); }
constexpr Vec3 Norm3::cross(const Vec3& v) const noexcept {
    const auto x = y_ * v.z() - z_ * v.y();
    const auto y = z_ * v.x() - x_ * v.z();
    const auto z = x_ * v.y() - y_ * v.x();
    return Vec3(x, y, z);
}
constexpr Vec3 Norm3::cross(const Norm3& v) const noexcept { 
	
	return cross(Vec3(v)); 
}

constexpr Norm3 Norm3::reflect(const Norm3& incoming) const noexcept {
    // We know by construction that the result of this operation is a normal. Says Matt.
    return Norm3(incoming.toVec3() - toVec3() * 2 * dot(incoming));
}

inline Norm3 Norm3::rotate(const Norm3& rotationAxis, double alpha) noexcept {
    // https://en.wikipedia.org/wiki/Rotation_matrix
    const auto x = *this;
    return (rotationAxis * rotationAxis.dot(x) + cos(alpha) * rotationAxis.cross(x).cross(rotationAxis) + sin(alpha) * rotationAxis.cross(x)).norm();
}

inline std::pair<Norm3, Norm3> Norm3::orthogonal_from_z(const Norm3& z) noexcept {
    auto x = (Norm3::xAxis().dot(z) < 1 - constants::EPS ? Norm3::xAxis() : Norm3::yAxis()).cross(z).norm();
    auto y = z.cross(x).norm();
    return { x,y };
}
inline std::array<Norm3, 3> Norm3::orthogonalFromYZ(const Norm3& y, const Norm3& z) noexcept {
    const auto x = y.cross(z).norm();
    const auto zz = Norm3::knownNormal(x.cross(y));
    return { x,y,zz };
}

inline std::array<Norm3, 3> Norm3::orthogonalFromZY(const Norm3& z, const Norm3& y) noexcept {
    const auto x = y.cross(z).norm();
    const auto yy = Norm3::knownNormal(z.cross(x));
    return { x,yy,z };
}

constexpr Vec3 operator*(double lhs, const Norm3& rhs) noexcept { return Vec3(rhs).scale(lhs); }
constexpr Vec3 Norm3::operator*(double rhs) const noexcept { return Vec3(*this).scale(rhs); }