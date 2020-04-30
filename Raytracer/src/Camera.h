#pragma once
#include "norm3.h"
#include "Ray.h"
#include "Constants.h"
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <array>
#include <random>


// Basic Camera for the moment at the middle of the screen at X/2, Y/2
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
// Umrechnung Field of view angle vertical to horizontal: vertical == 2*atan( tan(FoV/2) / Aspect Ratio)
class Camera {
	double aspectRatio_;
	std::size_t pixelWidth_;
	std::size_t pixelHeight_;
	double cameraPlaneDist_; // in radians
	glm::dvec3 eye_;
	glm::dmat4 matTrans_;
	glm::dmat4 matOrient_;
	glm::dmat4 matLookAt_;
	std::array<norm3,3> basis_;

	[[nodiscard]] glm::vec2 normalizedPosition(std::size_t pixX, std::size_t pixY, std::mt19937& rnd) const;

public:
	Camera(std::size_t pixelWidth, std::size_t PixelHeight, const glm::dvec3& eye, const glm::dvec3& motive, const norm3& up, double FOV);  // FOV input in degrees

	[[nodiscard]] norm3 getRayDirection(std::size_t X, std::size_t Y, std::mt19937& rnd) const;

	[[nodiscard]] inline Ray getRay(std::size_t X, std::size_t Y, std::mt19937& rnd) const {
		return { eye_,getRayDirection(X,Y,rnd) };
	}
};