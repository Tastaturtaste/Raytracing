#pragma once
#include "Vec3.h"
#include "Norm3.h"
#include "Ray.h"
#include "Constants.h"
#include <cmath>
#include <random>

// Basic Camera for the moment at the middle of the screen at X/2, Y/2

class Camera {
	double aspectRatio_;
	unsigned int width_;
	unsigned int height_;
	double FOV_; // in radians
	double CameraPlaneDist_;

	template<typename Rnd_dev>
	[[nodiscard]] std::pair<double, double> normalizeCanvas(unsigned int pixX, unsigned int pixY, Rnd_dev rnd) {
		std::uniform_real_distribution uni;
		const double x = (pixX + uni(rnd)) / width_;
		const double y = (pixY + uni(rnd)) / height_;
		return { (2*x-1.0),(2*y-1.0) };
	}

public:
	Camera(unsigned int width, unsigned int height, double FOV) noexcept // FOV input in degrees
		: aspectRatio_(static_cast<double>(width)/height), width_(width), height_(height), FOV_(FOV * constants::pi / 180.0) {
		CameraPlaneDist_ = aspectRatio_ / tan(FOV_ / 2);
	}

	template<typename RndDev>
	[[nodiscard]] Norm3 getRayDirection(unsigned int X, unsigned int Y, RndDev rnd) {
		const auto xy = normalizeCanvas<RndDev>(X, Y, rnd);
		const auto x = xy.first;
		const auto y = xy.second;
		const auto rx = x * aspectRatio_;
		const auto ry = y;
		const auto rz = CameraPlaneDist_;
		return Vec3(rx, ry, rz).norm();
	}
	
};