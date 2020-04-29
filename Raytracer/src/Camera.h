#pragma once
#include "norm3.h"
#include "Ray.h"
#include "Constants.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <cmath>
#include <array>
#include <random>

// Basic Camera for the moment at the middle of the screen at X/2, Y/2
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
// Umrechnung Field of view angle vertical to horizontal: vertical == 2*atan( tan(FoV/2) / Aspect Ratio)
class Camera {
	double aspectRatio_;
	unsigned int pixelWidth_;
	unsigned int pixelHeight_;
	double cameraPlaneDist_; // in radians
	glm::dvec3 eye_;
	glm::dmat4 matTrans_;
	glm::dmat4 matOrient_;
	glm::dmat4 matLookAt_;
	std::array<norm3,3> basis_;

	[[nodiscard]] glm::vec2 normalizedPosition(unsigned int pixX, unsigned int pixY, std::mt19937& rnd) const {
		const std::uniform_real_distribution uni;
		const double x = (pixX + uni(rnd)) / pixelWidth_;
		const double y = (pixY + uni(rnd)) / pixelHeight_;
		const double xTransform = (2 * x - 1.0) / cameraPlaneDist_ * aspectRatio_;
		const double yTransform = (2 * y - 1.0) / cameraPlaneDist_;
		return { xTransform,yTransform };
	}

public:
	Camera(unsigned int pixelWidth, unsigned int PixelHeight, glm::dvec3 eye, glm::dvec3 motive, norm3 up, double FOV)  // FOV input in degrees
		: aspectRatio_(static_cast<double>(pixelWidth) / PixelHeight), pixelWidth_(pixelWidth), pixelHeight_(PixelHeight), 
		eye_(eye), matLookAt_(glm::lookAt(eye,motive,up.getVec())), matTrans_(glm::translate(glm::dmat4(1.0),eye)), matOrient_(glm::orientation(glm::normalize(eye - motive),up.getVec())), 
		basis_(norm3::orthogonalFromZY(norm3(motive - eye),up)),	cameraPlaneDist_(aspectRatio_ / tan(FOV / 2)) {}

	[[nodiscard]] norm3 getRayDirection(unsigned int X, unsigned int Y, std::mt19937& rnd) const {
		const auto xy = normalizedPosition(X, Y, rnd);
		const auto x = xy.x;
		const auto y = xy.y;
		// Raydirection in CameraSpace
		const norm3 dirCamSpace = norm3(glm::dvec3(x, y, -1.0));
		const auto dirWorldSpace = glm::inverse(matLookAt_) * glm::vec4(dirCamSpace.getVec(), 0.0);
		auto dir = norm3(glm::dvec3(dirWorldSpace));
		return dir;
	}
	[[nodiscard]] Ray getRay(unsigned int X, unsigned int Y, std::mt19937& rnd) const {
		return { eye_,getRayDirection(X,Y,rnd) };
	}
};