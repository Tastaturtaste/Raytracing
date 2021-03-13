#pragma once
#include "Ray.h"
#include "Color.h"
#include "IntersectionTrace.h"
#include "spdlog\spdlog.h"

// Collection of Bidirectional Reflectance Distribution Function and Bidirectional Transmission Distribution Functions
namespace bxdf {

	enum class reflectionType { diffuse = 0, specular };

	struct bxdfResult {
		norm3 out{};
		double pdf{1.};
		Color attenuation{0.,0.,0.};
	};

	inline norm3 transform(const std::array<norm3, 3>& basis, const glm::dvec3& vec) {
		return norm3(basis[0].getVec() * vec.x + basis[1].getVec() * vec.y + basis[2].getVec() * vec.z);
	}

	inline bxdfResult lambert(const std::array<norm3, 3>& basis, const Ray& in, const IntersectionTrace& trace, double u, double v) {
		// Cosine distributed importance sampling, therefor pdf is one.
		const auto theta = 2 * constants::pi * u;
		const auto radius = glm::sqrt(v);
		const auto x = radius * glm::cos(theta);
		const auto z = radius * glm::sin(theta);
		const auto y = glm::sqrt(1.0 - x * x - z * z);
		return { .out = transform(basis, glm::dvec3(x, y, z)), .pdf = 1.0, .attenuation = trace.material->diffuse_color };
	}

	inline bxdfResult phong(const std::array<norm3, 3>& basis, const Ray& in, const IntersectionTrace& trace, double u, double v) {
		const auto Ns = trace.material->specularExponent;
		const auto alpha = glm::acos(glm::pow(u, 1. / (Ns + 1.)));
		const auto theta = 2 * constants::pi * v;
		const auto radius = glm::sin(alpha);
		const auto x = radius * glm::cos(theta);
		const auto z = radius * glm::sin(theta);
		const auto y = glm::sqrt(1.0 - x * x - z * z);
		const auto sample = glm::dvec3(x, y, z);
		// TODO: Check if pdf of 1.0 is correct for this importance sampling
		return { .out = transform(basis, glm::dvec3(x, y, z)), .pdf = 1.0, .attenuation = trace.material->specular_color };
	}

	inline bxdfResult bxdf(const Ray& in, const IntersectionTrace& trace, double u, double v, double w) {
		if(w < trace.material->prob_diffuse) {
			const std::array<norm3, 3> base = norm3::orthogonalFromY(trace.normal);
			return lambert(base, in, trace, u, v);
		}
		else {
			const std::array<norm3, 3> base = norm3::orthogonalFromY(norm3(glm::reflect(in.direction().getVec(), trace.normal.getVec())));
			return phong(base, in, trace, u, v);
		}
	}
}