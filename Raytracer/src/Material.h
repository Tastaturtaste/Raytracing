#pragma once
#include "Color.h"

class Material
{
	Color diffuse_color_{};
	Color specular_color_{};
	Color light_color_{};
	float prob_diffuse_{};

public:
	constexpr Material(Color diff_color, Color spec_color, Color light_color, float prob_diffuse) noexcept
		: diffuse_color_(diff_color), specular_color_(spec_color), light_color_(light_color), prob_diffuse_(prob_diffuse) {}
	constexpr Material() = default;

	constexpr Color diffuse_color() const noexcept { return diffuse_color_; }
	constexpr Color specular_color() const noexcept { return specular_color_; }
	constexpr Color light_color() const noexcept { return light_color_; }
	constexpr float prob_diffuse() const noexcept { return prob_diffuse_; }

	static Material white_light(double power) noexcept { return Material({}, {}, { power,power,power }, 1.0f); }
	static Material green_diffuse() noexcept { return Material({ 0.2,0.8,0.2 }, { 0.2,0.8,0.2 }, {}, 0.9f); }
	static Material red_shiny() noexcept { return Material({ 0.8,0.2,0.2 }, { 0.8,0.2,0.2 }, {}, 0.1f); }
	static Material mirror() noexcept { return Material({ 0.999,0.999,0.999 }, { 0.999,0.999,0.999 }, {}, 0.0f); }
	static Material grey_diffuse(double greyVal) noexcept { 
		return Material({ greyVal, greyVal, greyVal }, { greyVal,greyVal,greyVal }, {}, 1.0f); 
	}
};
