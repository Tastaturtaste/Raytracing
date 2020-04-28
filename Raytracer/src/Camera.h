#pragma once
#include "Vec3.h"
#include "Norm3.h"
#include "Ray.h"
#include "Constants.h"
#include <cmath>
#include <array>
#include <random>

// Basic Camera for the moment at the middle of the screen at X/2, Y/2

class Camera {
	double aspectRatio_;
	unsigned int width_;
	unsigned int height_;
	double FOV_; // in radians
	double CameraPlaneDist_;
	Vec3 position_;
	std::array<Norm3,3> basis_;

	template<typename Rnd_dev>
	[[nodiscard]] std::pair<double, double> normalizeCanvas(unsigned int pixX, unsigned int pixY, Rnd_dev& rnd) const {
		const std::uniform_real_distribution uni;
		const double x = (pixX + uni(rnd)) / width_;
		const double y = (pixY + uni(rnd)) / height_;
		return { (2*x-1.0),(2*y-1.0) };
	}

public:
	Camera(unsigned int width, unsigned int height, Vec3 position, Vec3 motive, double FOV) noexcept // FOV input in degrees
		: aspectRatio_(static_cast<double>(width) / height), width_(width), height_(height), 
		CameraPlaneDist_(aspectRatio_ / tan(FOV_ / 2)), position_(position), basis_(Norm3::orthogonalFromZY((motive - position).norm(),Norm3::yAxis())),
		FOV_(FOV* constants::pi / 180.0) {}

	template<typename RndDev>
	[[nodiscard]] Norm3 getRayDirection(unsigned int X, unsigned int Y, RndDev& rnd) const {
		const auto xy = normalizeCanvas<RndDev>(X, Y, rnd);
		const auto x = xy.first;
		const auto y = xy.second;
		const auto rx = x * aspectRatio_;
		const auto ry = y;
		const auto rz = CameraPlaneDist_;
		return Vec3(rx, ry, rz).norm();
	}
	template<typename RndDev>
	[[nodiscard]] Ray getRay(unsigned int X, unsigned int Y, RndDev& rnd) const {
		const auto xy = normalizeCanvas(X, Y, rnd);
		const double x = xy.first;
		const double y = xy.second;
		const auto rx = x * aspectRatio_ * basis_[0];
		const auto ry = y * basis_[1];
		const auto rz = basis_[2] * CameraPlaneDist_;
		const auto direction = Vec3(rx + ry + rz).norm();
		return { position_,direction };
	}
};