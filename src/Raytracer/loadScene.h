#pragma once
#include <glm\glm.hpp>
#include "Scene.h"
#include "util/utilities.h"
#include <istream>
#include <span>
#include <unordered_map>

namespace Tracer {
template<class T = double>
glm::vec<3, T> parse_vertex(std::span<std::string_view> tokens) {
	return { asNumber<T>(tokens[0]), asNumber<T>(tokens[1]), asNumber<T>(tokens[2]) };
}
std::vector<std::string_view> splitString(std::string_view line, std::string_view delims);
std::vector<size_t> parse_indices(std::string_view line, size_t num_vertices);
std::unordered_map<std::string, std::unique_ptr<Material>> loadMaterials(std::istream& mtl_file);
Scene loadScene(std::istream& obj_file);
} // Tracer
