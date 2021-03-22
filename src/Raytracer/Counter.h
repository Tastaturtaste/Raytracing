#pragma once
#include <mutex>

struct Counter {
	unsigned int i{};
	std::mutex m{};
	unsigned int operator++(int) {
		const std::lock_guard<std::mutex> lock(m);
		const auto temp = i;
		++i;
		return temp;
	}
	unsigned int get() {
		const std::lock_guard<std::mutex> lock(m);
		return i;
	}
	//explicit operator unsigned int() const { return i; }
};