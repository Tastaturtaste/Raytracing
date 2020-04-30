#pragma once
#include "Color.h"

class Material
{
	Color diffuse_color_{};
	Color specular_color_{};
	Color light_color_{};
	double prob_diffuse_{};

public:
	constexpr Material(Color diff_color, Color spec_color, Color light_color, double prob_diffuse) noexcept
		: diffuse_color_(diff_color), specular_color_(spec_color), light_color_(light_color), prob_diffuse_(prob_diffuse) {}
	constexpr Material() = default;

	constexpr Color diffuse_color() const noexcept { return diffuse_color_; }
	constexpr Color specular_color() const noexcept { return specular_color_; }
	constexpr Color light_color() const noexcept { return light_color_; }
	constexpr double prob_diffuse() const noexcept { return prob_diffuse_; }

	static Material white_light(double power) noexcept { return Material({}, {}, Color( power,power,power ), 1.0f); }
	static Material green_diffuse() noexcept { return Material(Color( 0.1,0.4,0.1 ), Color( 0.1,0.4,0.1 ), {}, 0.9f); }
	static Material red_shiny() noexcept { return Material(Color( 0.8,0.2,0.2 ), Color( 0.8,0.2,0.2 ), {}, 0.1f); }
	static Material mirror() noexcept { return Material(Color( 0.999,0.999,0.999 ), Color( 0.999,0.999,0.999 ), {}, 0.0f); }
	static Material grey_diffuse(double greyVal) noexcept { 
		return Material(Color( greyVal, greyVal, greyVal ), Color( greyVal,greyVal,greyVal ), {}, 1.0f); 
	}
};
