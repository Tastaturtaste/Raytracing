#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

constexpr double asRad(double deg) noexcept {
	return constants::pi * deg / 180.;
}

inline glm::dvec2 Camera::cameraToPixel(glm::dvec3 camCoord) const {
	return glm::inverse(projectionMat_) * glm::dvec4(camCoord, 1.);
}

inline glm::dvec3 Camera::pixelToCamera(glm::dvec2 pixCoord) const {
	return projectionMat_ * glm::vec4(pixCoord.x, pixCoord.y, 0., 1.);
}

inline glm::dvec4 Camera::worldToCamera(glm::dvec4 worldCoord) const {
	return viewMat_ * worldCoord;
}

inline glm::dvec4 Camera::cameraToWorld(glm::dvec4 camCoord) const {
	return glm::inverse(viewMat_) * camCoord;
}

Ray Camera::getRay(glm::dvec2 pixCoord, std::mt19937& rnd) const {
	std::uniform_real_distribution<double> const uni{ 0., 1. };
	auto const aaPix = glm::dvec2(pixCoord.x + uni(rnd), pixCoord.y + uni(rnd));
	// Transform pixel into camera space
	auto const camPixCoord = pixelToCamera(aaPix);
	glm::dvec4 origin = { 0.,0.,0.,1. };
	// Since origin is as (0,0,0) in cam space no need to subtract it to get direction
	glm::dvec4 direction = glm::dvec4(camPixCoord, 0.);
	// translate origin to into pixel plane so objects between the camera eye and the plane cannot intersect
	//origin = glm::translate(glm::dmat4(1.), camPixCoord) * origin;
	// Transform origin and direction to world coordinates
	origin = cameraToWorld(origin);
	direction = cameraToWorld(direction);
	Ray r{ glm::dvec3(origin), norm3(direction) };
	return r;
}

Camera::Camera(std::size_t pixelWidth, std::size_t pixelHeight, glm::dvec3 eye, glm::dvec3 motive, norm3 up, double FOV)
	: aspectRatio_(static_cast<double>(pixelWidth) / pixelHeight), FOV_(FOV), camPlaneDistance_(1. / glm::tan(FOV_ / 2)),
	pixelWidth_(pixelWidth), pixelHeight_(pixelHeight), eye_(eye), viewMat_(glm::lookAt(eye_,motive,up.getVec()))
{
	auto const trans = glm::translate(glm::dmat4(1.), glm::dvec3(-static_cast<double>(pixelWidth_) / 2., -static_cast<double>(pixelHeight_) / 2., -camPlaneDistance_));
	auto const scale = glm::scale(glm::dmat4(1.), glm::dvec3(2. / pixelWidth_ * aspectRatio_, 2. / pixelHeight_, 1.));
	projectionMat_ = scale * trans;
}