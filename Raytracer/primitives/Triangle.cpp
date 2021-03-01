#include "Triangle.h"
#include <cmath>
#include <bitset>
#include <glm\glm.hpp>

std::optional<IntersectionTrace> Triangle::intersect_impl(const Ray& r) const noexcept {
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

	const glm::dvec3 N = glm::cross(edgeFromTo<0, 1>(), edgeFromTo<0, 2>());	// not normalized!
	//double const areaTimesTwo = N.length();
	//N = N / areaTimesTwo;

	const double NdotRd = glm::dot(r.direction().getVec(), N);
	// NdotRd ~ 0 -> ray parallel to triangle
	// NdotRd > 0 -> ray points in same direction as normal -> ray from inside
	// NdotRd < 0 -> ray points against normal -> ray from outside
	if (std::abs(NdotRd) < constants::EPS)
		return {};	// Parallel
	const double d = glm::dot(N, vertices_[0]);
	const double t = -(glm::dot(N, r.origin()) + d) / NdotRd;
	if (t < constants::EPS) return {};	// Ray origin behind triangle
	const glm::dvec3 hitPoint = r.positionAlong(t);
	// check for each leg if hitpoint is on the left. Vertices are expected to be in mathmatical order / against the clock
	std::bitset<3> isRight(0);
	const glm::dvec3 pv0 = hitPoint - vertices_[0];
	const glm::dvec3 pv1 = hitPoint - vertices_[1];
	const glm::dvec3 pv2 = hitPoint - vertices_[2];
	isRight.set(0, glm::dot(N, glm::cross(edgeFromTo<0, 1>(), pv0)) < 0);
	isRight.set(1, glm::dot(N, glm::cross(edgeFromTo<1, 2>(), pv1)) < 0);
	isRight.set(2, glm::dot(N, glm::cross(edgeFromTo<2, 0>(), pv2)) < 0);
	if (isRight.any())
		return {};
	return { {.hit_position = hitPoint, .normal = normal_, .distance = t, .material = material(), .inside = false } };
}

//std::optional<IntersectionTrace> Triangle::intersect_impl(const Ray& r) const noexcept {
//	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
//
//	const glm::dvec3 N = glm::cross(edgeFromTo<0,1>(),edgeFromTo<0,2>());	// not normalized!
//	//double area2 = N.len();
//	
//	const double NdotRd = glm::dot(r.direction().getVec(),N);
//	if (std::abs(NdotRd) < constants::EPS)
//		return {};	// Parallel
//	const double d = glm::dot(N,vertices_[0]);
//	const double t = (glm::dot(N,r.origin()) + d) / NdotRd;
//	if (t < constants::EPS) return {};	// Reyorigin behind triangle
//	const glm::dvec3 hitPoint = r.positionAlong(t);
//	// check for each leg if hitpoint is on the left. Vertices are expected to be in mathmatical order / against the clock
//	std::bitset<3> isRight(0);
//	const glm::dvec3 pv0 = hitPoint - vertices_[0];
//	const glm::dvec3 pv1 = hitPoint - vertices_[1];
//	const glm::dvec3 pv2 = hitPoint - vertices_[2];
//	isRight.set(0, glm::dot(N,glm::cross(edgeFromTo<0, 1>(),pv0)) < 0);
//	isRight.set(1, glm::dot(N, glm::cross(edgeFromTo<1, 2>(), pv1)) < 0);
//	isRight.set(2, glm::dot(N, glm::cross(edgeFromTo<2, 0>(), pv2)) < 0);
//	if (isRight.none() || isRight.all()) 
//		return { {  .hit_position = hitPoint, .normal = normal_, .distance = t, .material = material(), .inside = false } };
//	else
//		return {};
//}
