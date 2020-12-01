#pragma once
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>
#include <charconv>
#include "glm\glm.hpp"
#include "ctre.hpp"

namespace Tracer {
	constexpr auto comment_re = ctll::fixed_string{ R"(.*?#.*)" };
	constexpr auto vertex_re = ctll::fixed_string{ R"(^v\s+(.+)\s+(.+)\s+(.+)\.*)" };
	constexpr auto texture_re = ctll::fixed_string{ R"(^vt\s+(.+)\s+(.+)\.*)" };
	constexpr auto normal_re =  ctll::fixed_string{ R"(^vn\s+(.+)\s+(.+)\s+(.+)\.*)" };

	template<class T = double>
	glm::vec<3, T> parse_vertex(std::string_view line) {
		auto [sv, sv1, sv2, sv3] = ctre::match<vertex_re>(line);
		glm::vec<3, T> vec{};
		std::from_chars(sv1.to_view().data(), sv1.to_view().data() + sv1.size(), vec[0]);
		std::from_chars(sv2.to_view().data(), sv2.to_view().data() + sv1.size(), vec[1]);
		std::from_chars(sv3.to_view().data(), sv3.to_view().data() + sv1.size(), vec[2]);
		return vec;
	}
	template<class T = double>
	glm::vec<3, T> parse_texture(std::string_view line) {
		auto [sv, sv1, sv2, sv3] = ctre::match<texture_re>(line);
		glm::vec<3, T> vec{};
		std::from_chars(sv1.to_view().data(), sv1.to_view().data() + sv1.size(), vec[0]);
		std::from_chars(sv2.to_view().data(), sv2.to_view().data() + sv1.size(), vec[1]);
		std::from_chars(sv3.to_view().data(), sv3.to_view().data() + sv1.size(), vec[2]);
		return vec;
	}
	template<class T = double>
	glm::vec<3, T> parse_normal(std::string_view line) {
		auto [sv, sv1, sv2, sv3] = ctre::match<normal_re>(line);
		glm::vec<3, T> vec{};
		std::from_chars(sv1.to_view().data(), sv1.to_view().data() + sv1.size(), vec[0]);
		std::from_chars(sv2.to_view().data(), sv2.to_view().data() + sv1.size(), vec[1]);
		std::from_chars(sv3.to_view().data(), sv3.to_view().data() + sv1.size(), vec[2]);
		return vec;
	}
}
