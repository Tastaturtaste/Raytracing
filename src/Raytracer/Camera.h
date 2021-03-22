#pragma once
#include "norm3.h"
#include "Ray.h"
#include "util/Constants.h"
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <array>
#include <random>


// Basic Camera for the moment at the middle of the screen at X/2, Y/2
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
// Umrechnung Field of view angle vertical to horizontal: vertical == 2*atan( tan(FoV/2) / Aspect Ratio)
class Camera {
	double aspectRatio_{};
	double FOV_{}; // in radians
	double camPlaneDistance_{};
	std::size_t pixelWidth_{};
	std::size_t pixelHeight_{};
	glm::dvec3 eye_{};
	glm::dmat4 viewMat_{}; // Transforms from world to camera space
	glm::dmat4 projectionMat_{}; // Transforms from camera to screen space

	[[nodiscard]] glm::dvec2 cameraToPixel(glm::dvec3 camCoord) const;
	[[nodiscard]] glm::dvec3 pixelToCamera(glm::dvec2 pixCoord) const;
	[[nodiscard]] glm::dvec4 cameraToWorld(glm::dvec4 camCoord) const;
	[[nodiscard]] glm::dvec4 worldToCamera(glm::dvec4 worldCoord) const;

public:
	Camera(std::size_t pixelWidth, std::size_t PixelHeight, glm::dvec3 eye, glm::dvec3 motive, norm3 up, double FOV);  // FOV input in degrees


	[[nodiscard]] Ray getRay(glm::dvec2 pixCoord, double u, double v) const;
};