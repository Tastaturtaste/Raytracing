#pragma once
#include <vector>
#include <array>
#include <span>
#include <optional>
#include <algorithm>

#include <glm\glm.hpp>
#include <gsl-lite.hpp>

#include "primitives/BoundingBox.h"
#include "primitives/Geometry.h"
#include "util/utilities.h"


constexpr BoundingBox sphereBB(Sphere const& sphere) {
	return { sphere.origin() - glm::dvec3(sphere.radius()), sphere.origin() + glm::dvec3(sphere.radius()) };
}

constexpr uint8_t cmprPoints(glm::dvec3 const& point1, glm::dvec3 const& point2) noexcept {
	// bitencoding of octant
	return (point1.x > point2.x) + 
		( (point1.y > point2.y) << 1 ) + 
		( (point1.z > point2.z) << 2 );
}

inline int16_t cmprGeometryToPoint(Geometry const& geo, glm::dvec3 const& point) {
	// If axis aligned surfaces defined by point do not cut bounding box of geometry returns bitencoding of octant of geometry
	// Else returns -1
	BoundingBox const box{ geo.boundingBox() };
	const auto cmpr1 = cmprPoints(box.min, point);
	const auto cmpr2 = cmprPoints(box.max, point);
	return cmpr1 == cmpr2 ? cmpr1 : -1;
}

inline BoundingBox boundingBoxFromGeometries(std::span<Geometry> geos) {
	auto it = geos.begin();
	BoundingBox bb{ it->boundingBox() };
	for (++it; it != geos.end(); ++it) {
		bb = combineBoundingBoxes(bb, it->boundingBox());
	}
	return bb;
}

class OcTree {

	struct OcNode {
		BoundingBox bb{};
		std::size_t begin{}; // first index with geometries for this node
		std::size_t end{}; // one past last index with geometries for this node
		std::size_t parent_idx{};
		std::array<std::size_t, 8> children_idx{}; // children_idx of zero means no further nodes in that quadrant since idx zero is reserved for the top level node

		std::optional<IntersectionTrace> nearestIntersect(OcTree const& tree, Ray const& ray) const;
	};
	std::size_t linearSize{1};
	std::vector<Geometry> geos{};
	std::vector<OcNode> nodes{};

	std::size_t makeOcNodes(std::size_t begin, std::size_t end, std::size_t parent_idx) {
		// Returns a index into the nodes vector indicating the current vector
		auto const itBegin = geos.begin() + begin;
		auto const itEnd = geos.begin() + end;
		BoundingBox const bb = boundingBoxFromGeometries({ itBegin, itEnd });
		if (end - begin <= linearSize) {
			// End recursion 
			nodes.push_back(OcNode{ bb, begin, end, parent_idx });
			return nodes.size() - 1;
		}
		// Find midpoint of bounding box of current node
		auto bbMidPoint = (bb.max + bb.min) / 2.;
		// Sort geometries into octants
		std::sort(itBegin, itEnd, [&](auto const& geo1, auto const& geo2) {
			return cmprGeometryToPoint(geo1, bbMidPoint) < cmprGeometryToPoint(geo2, bbMidPoint);
			});
		// Find first element that can be sorted cleanly into one octant
		auto curItEnd = std::find_if(itBegin, itEnd, [&](auto const& geo) { return cmprGeometryToPoint(geo, bbMidPoint) > -1; });
		std::size_t childBegin = gsl::narrow_cast<std::size_t>(std::distance(geos.begin(), curItEnd));
		nodes.push_back(OcNode{ bb, begin, childBegin, parent_idx });
		// Save index of inserted node to return later
		std::size_t const currentIndex = nodes.size() - 1;

		for (auto i = 0; i < 8; ++i) {
			curItEnd = std::find_if_not(curItEnd, itEnd, [&](auto geo) { return cmprGeometryToPoint(geo, bbMidPoint) < i+1; });
			auto childEnd = gsl::narrow_cast<std::size_t>(std::distance(geos.begin(), curItEnd));
			if (childEnd - childBegin > 0) {
				// If no geometries are in this octant no child has to be made
				nodes.at(currentIndex).children_idx[i] = makeOcNodes(childBegin, childEnd, currentIndex);
			}
			else {
				nodes.at(currentIndex).children_idx[i] = 0;
			}
			childBegin = childEnd;
		}
		return currentIndex;
	}

public:
	inline std::optional<IntersectionTrace> intersect(Ray const& ray) const {
		return nodes[0].nearestIntersect(*this, ray);
	}
	OcTree(std::vector<Geometry> geometries, std::size_t linearSize = 1) : linearSize(linearSize), geos(std::move(geometries)) {
		assert(linearSize > 0);
		makeOcNodes(0, geos.size(), -1);
	}
	constexpr std::vector<Geometry> const& getGeometries() const noexcept { return geos; }
};

inline std::optional<IntersectionTrace> OcTree::OcNode::nearestIntersect(OcTree const& tree, Ray const& ray) const {
	// Check if bounding box of current node is hit
	if (!bb.intersect(ray)) {
		return {};
	}
	std::optional<IntersectionTrace> nearest;
	double min_dist = std::numeric_limits<double>::infinity();
	// Find nearest intersection in current node
	for( auto it = tree.geos.begin() + begin; it != tree.geos.begin() + end; ++it){
		auto temp = it->intersect(ray);
		if (temp.has_value() && temp->distance < min_dist) {
			min_dist = temp->distance;
			nearest.emplace(std::move(temp.value()));
		}
	}
	// Check children for intersections
	for (auto it = children_idx.begin(); it != children_idx.end(); ++it) {
		if (*it == 0) {
			// Skip zero since this index always holds the root node
			continue;
		}
		auto temp = tree.nodes[*it].nearestIntersect(tree, ray);
		if (temp.has_value() && temp->distance < min_dist) {
			nearest.emplace(std::move(temp.value()));
		}
	}
	return nearest;
}