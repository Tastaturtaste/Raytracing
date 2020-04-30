#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

glm::vec2 Camera::normalizedPosition(std::size_t pixX, std::size_t pixY, std::mt19937& rnd) const {
	const std::uniform_real_distribution uni;
	const double x = (pixX + uni(rnd)) / pixelWidth_;
	const double y = (pixY + uni(rnd)) / pixelHeight_;
	const double xTransform = (2 * x - 1.0) / cameraPlaneDist_ * aspectRatio_;
	const double yTransform = (2 * y - 1.0) / cameraPlaneDist_;
	return { xTransform,yTransform };
}

norm3 Camera::getRayDirection(std::size_t X, std::size_t Y, std::mt19937& rnd) const {
	const auto xy = normalizedPosition(X, Y, rnd);
	const auto x = xy.x;
	const auto y = xy.y;
	// Raydirection in CameraSpace
	const norm3 dirCamSpace = norm3(glm::dvec3(x, y, -1.0));
	const auto dirWorldSpace = glm::inverse(matLookAt_) * glm::vec4(dirCamSpace.getVec(), 0.0);
	auto dir = norm3(glm::dvec3(dirWorldSpace));
	return dir;
}

Camera::Camera(std::size_t pixelWidth, std::size_t PixelHeight, const glm::dvec3& eye, const glm::dvec3& motive, const norm3& up, double FOV)  // FOV input in degrees
: aspectRatio_(static_cast<double>(pixelWidth) / PixelHeight), pixelWidth_(pixelWidth), pixelHeight_(PixelHeight),
eye_(eye), matLookAt_(glm::lookAt(eye, motive, up.getVec())), matTrans_(glm::translate(glm::dmat4(1.0), eye)), matOrient_(glm::orientation(glm::normalize(eye - motive), up.getVec())),
basis_(norm3::orthogonalFromZY(norm3(motive - eye), up)), cameraPlaneDist_(aspectRatio_ / tan(FOV / 2)) {}