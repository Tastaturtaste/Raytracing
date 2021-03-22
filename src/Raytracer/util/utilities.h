#pragma once
#include <string_view>
#include <charconv>
#include <stdexcept>
#include <span>
#include <concepts>

#include "glm\glm.hpp"

template<class T = double>
T asNumber(std::string_view sv) {
	T val;
	auto [p, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
	if (ec != std::errc()) {
		throw std::runtime_error("Could not convert string to number!");
	}
	return val;
}

template<class T = double>
constexpr glm::vec<3, T> elemwiseMin(glm::vec<3, T> vec1, glm::vec<3, T> vec2) noexcept {
	return glm::vec<3, T>(glm::min(vec1.x, vec2.x), glm::min(vec1.y, vec2.y), glm::min(vec1.z, vec2.z));
}

template<class T = double>
constexpr glm::vec<3, T> elemwiseMax(glm::vec<3, T> vec1, glm::vec<3, T> vec2) noexcept {
	return glm::vec<3, T>(glm::max(vec1.x, vec2.x), glm::max(vec1.y, vec2.y), glm::max(vec1.z, vec2.z));
}

template<class T = double>
glm::vec<3, T> toGlm(std::span<T> span) {
	if (span.size() != 3) {
		throw std::runtime_error("Cannot convert argument to glm vector! Number of elements unequal 3!");
	}
	return glm::vec<3, T>(span[0], span[1], span[2]);
}