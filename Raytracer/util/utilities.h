#pragma once
#include <string_view>
#include <charconv>
#include <stdexcept>

template<class T = double>
T asNumber(std::string_view sv) {
	T val;
	auto [p, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
	if (ec != std::errc()) {
		throw std::runtime_error("Could not convert string to number!");
	}
	return val;
}