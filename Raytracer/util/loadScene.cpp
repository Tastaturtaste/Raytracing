#include "loadScene.h"

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <charconv>
#include <span>
#include "spdlog\spdlog.h"
#include "glm\glm.hpp"
#include "ctre.hpp"
#include "Scene.h"
#include "Geometry.h"
#include "Material.h"

namespace Tracer {
constexpr auto comment_re = ctll::fixed_string{ R"(.*?#.*)" };
constexpr auto vertex_re = ctll::fixed_string{ R"(^v\s+(.+)\s+(.+)\s+(.+)\.*)" };
constexpr auto texture_re = ctll::fixed_string{ R"(^vt\s+(.+)\s+(.+)\.*)" };
constexpr auto normal_re = ctll::fixed_string{ R"(^vn\s+(.+)\s+(.+)\s+(.+)\.*)" };

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
glm::vec<3, T> parse_vertex(std::span<std::string_view> tokens) {
	return { asNumber<T>(tokens[0]), asNumber<T>(tokens[1]), asNumber<T>(tokens[2]) };
}
//template<class T = double>
//glm::vec<3, T> parse_texture(std::string_view line) {
//	auto [sv, sv1, sv2, sv3] = ctre::match<texture_re>(line);
//	glm::vec<3, T> vec{};
//	std::from_chars(sv1.to_view().data(), sv1.to_view().data() + sv1.size(), vec[0]);
//	std::from_chars(sv2.to_view().data(), sv2.to_view().data() + sv2.size(), vec[1]);
//	std::from_chars(sv3.to_view().data(), sv3.to_view().data() + sv3.size(), vec[2]);
//	return vec;
//}
//template<class T = double>
//glm::vec<3, T> parse_normal(std::string_view line) {
//	auto [sv, sv1, sv2, sv3] = ctre::match<normal_re>(line);
//	glm::vec<3, T> vec{};
//	std::from_chars(sv1.to_view().data(), sv1.to_view().data() + sv1.size(), vec[0]);
//	std::from_chars(sv2.to_view().data(), sv2.to_view().data() + sv2.size(), vec[1]);
//	std::from_chars(sv3.to_view().data(), sv3.to_view().data() + sv3.size(), vec[2]);
//	return vec;
//}

// TODO: Does not work if one of delims is at the end of line
std::vector<std::string_view> splitString(std::string_view line, std::string_view delims) {
	std::vector<std::string_view> tokens;
	size_t tokenBegin = 0;
	size_t tokenEnd = 0;
	while (tokenEnd != line.size()) {
		tokenBegin = line.find_first_not_of(delims, tokenEnd);
		// Check if delimiter is at the end of line
		if (tokenBegin == line.npos) {
			break;
		}
		tokenEnd = line.find_first_of(delims, tokenBegin);
		// Check if line ends without delimiter
		if (tokenEnd == line.npos) {
			tokenEnd = line.size();
		}
		tokens.emplace_back(line.data() + tokenBegin, line.data() + tokenEnd);
	}
	return tokens;
}

std::vector<size_t> parse_indices(std::string_view line, size_t num_vertices) {
	std::vector<size_t> indices{};
	line = std::string_view(line.data() + line.find_first_not_of(" f"), line.data() + line.size());
	while (line.size() > 0) {
		int idx;
		auto [pos, ec] = std::from_chars(line.data(), line.data() + line.size(), idx);
		if (idx < 1) {
			idx = idx + num_vertices;
		}
		else {
			idx -= 1;
		}
		indices.push_back(idx);
		line = std::string_view(pos);
		auto nextBegin = line.find_first_not_of(" f");
		if (nextBegin == line.npos) {
			nextBegin = line.size();
		}
		line = std::string_view(line.data() + nextBegin ,line.data() + line.size() );
	}
	return indices;
}

static std::string_view cutAtDelimiter(std::string_view sv, char delim) {
	auto svEnd = sv.find(delim);
	if (svEnd == sv.npos) {
		svEnd = sv.size();
	}
	return std::string_view(sv.begin(), sv.begin() + svEnd );
}



std::unordered_map<std::string, std::unique_ptr<Material>> loadMaterials(std::istream& mtl_file) {
	if (!mtl_file) {
		spdlog::error("Bad input stream!");
		throw std::runtime_error("Bad input stream!");
	}
	mtl_file.exceptions(std::ios_base::badbit);
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	std::string line;
	int linenumber{};
	std::unique_ptr<Material> material;
	std::string mat_name;
	while (std::getline(mtl_file, line)) {
		++linenumber;
		std::string_view lineView = cutAtDelimiter(line, '#');
		auto tokens = splitString(lineView, " ");
		if (tokens.size() == 0) {
			continue;
		}
		if (tokens.at(0) == "newmtl") {
			if (tokens.size() != 2) {
				throw std::runtime_error("Wrong number of tokens for declaration of new material!");
			}
			mat_name = tokens.at(1);
			materials[mat_name] = std::make_unique<Material>();
		}
		else if (tokens.at(0) == "Kd") {
			if (tokens.size() != 4) {
				throw std::runtime_error("Wrong number of tokens for declaration of diffuse color!");
			}
			if (mat_name.size() == 0) {
				throw std::runtime_error("No material for the diffuse color specified!");
			}
			materials[mat_name]->diffuse_color = { asNumber<double>(tokens.at(1)), asNumber<double>(tokens.at(2)), asNumber<double>(tokens.at(3)) };
		}
		else if (tokens.at(0) == "Ke") {
			if (tokens.size() != 4) {
				throw std::runtime_error("Wrong number of tokens for declaration of emissive color!");
			}
			if (mat_name.size() == 0) {
				throw std::runtime_error("No material for the emissive color specified!");
			}
			materials[mat_name]->light_color = { asNumber<double>(tokens.at(1)), asNumber<double>(tokens.at(2)), asNumber<double>(tokens.at(3)) };
		}
		else if (tokens.at(0) == "Ns") {
			if (tokens.size() != 2) {
				throw std::runtime_error("Wrong number of tokens for declaration of specular coefficient!");
			}
			if (mat_name.size() == 0) {
				throw std::runtime_error("No material for the specular coefficient specified!");
			}
			// Ns typically in range [0,1000]
			auto val = std::clamp(asNumber<double>(tokens.at(1)) / 1000., 0.0, 1.0);
			materials[mat_name]->reflection_cone_radians = constants::pi * val;
		}
	}
	return materials;
}

Scene loadScene(std::istream& obj_file) {
	if (!obj_file) {
		spdlog::error("Bad input stream!");
		throw std::runtime_error("Bad input stream!");
	}
	obj_file.exceptions(std::ios_base::badbit);

	std::vector<glm::dvec3> vertices;
	std::string line;
	int linenumber{};
	std::vector<Geometry> geometries{};
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
	Material* curMat = nullptr;
	while (std::getline(obj_file, line)) {
		++linenumber;
		// remove possible comments, otherwise unchanged
		std::string_view lineView = cutAtDelimiter(line, '#');
		// Skip line if it is empty after comment removal
		if (lineView.size() == 0) {
			continue;
		}
		// Split line into tokens
		auto tokens = splitString(lineView, " \t");

		if (tokens.at(0) == "v") {
			if (tokens.size() != 4) {
				throw std::runtime_error("Wrong number of tokens for declaration of vertices!");
			}
			vertices.push_back(parse_vertex({ tokens.begin() + 1,tokens.end() }));
		}
		else if (tokens.at(0) == "f") {
			if (tokens.size() < 4) {
				throw std::runtime_error("Wrong number of vertices for a surface!");
			}
			auto indices = parse_indices(lineView, vertices.size());
			for (size_t i = 1; i < indices.size() - 1; ++i) {
				geometries.push_back(
					Triangle({ vertices.at(indices[0]), vertices.at(indices[i]), vertices.at(indices[i + 1]) }, 
						curMat)
				);
			}
		}
		else if (tokens.at(0) == "mtllib") {
			if (tokens.size() != 2) {
				throw std::runtime_error("Wrong number of tokens for declaration of mtl file!");
			}
			auto mtl_file = std::ifstream(std::string(tokens.at(1)));
			materials.merge(loadMaterials(mtl_file));
		}
		else if (tokens.at(0) == "usemtl") {
			if (tokens.size() != 2) {
				throw std::runtime_error("Wrong number of tokens for declaration of material using declaration!");
			}
			curMat = materials.at(std::string(tokens.at(1))).get();
		}
	}
	std::vector<std::unique_ptr<Material>> matVec{};
	matVec.reserve(materials.size());
	for (auto& [key, value] : materials) {
		matVec.push_back(std::move(value));
	}
	return Scene(std::move(geometries), std::move(matVec));
}
} // Tracer