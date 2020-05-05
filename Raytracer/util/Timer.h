#pragma once

#include <chrono>
#include <iostream>

/*Template argument has to be of type std::chrono::duration
If a pointer to int64_t is given as constructor argument, the lifetime of the timer object is stored by destruction.
Otherwise it gets printed to console.
*/
template<typename duration_type>
struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> begin;
	std::chrono::time_point<std::chrono::steady_clock> end;
	duration_type time{};
	long long* p_duration = nullptr;

	constexpr Timer() 
		: begin(std::chrono::high_resolution_clock::now()) {}
	constexpr explicit Timer(int64_t* duration) 
		: p_duration(duration),begin(std::chrono::high_resolution_clock::now())	{}

	constexpr void writeTime() noexcept {
		try
		{
			std::cout << "Scope was " << std::chrono::duration_cast<duration_type>(time).count() << " units alive.\n";
		}
		catch (const std::exception&){}
	}
	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration_cast<duration_type>(end - begin);
		if (p_duration)
			*p_duration = time.count();
		else
			writeTime();
	}
	Timer(Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;
};
