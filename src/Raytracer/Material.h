#pragma once
#include "Color.h"
#include "util/Constants.h"

struct Material
{
	Color diffuse_color{};
	Color specular_color{};
	Color light_color{};
	double prob_diffuse{1.0};
	double specularExponent{ 1. };

	constexpr Material() = default;
	constexpr Material(Color diff_color, Color specular_color, Color light_color, double prob_diffuse, double specularExponent) noexcept
		: diffuse_color(diff_color), specular_color(specular_color), light_color(light_color), prob_diffuse(prob_diffuse), specularExponent(specularExponent) {}

	static Material white_light(double power) noexcept { 
		return { {}, {}, Color(power,power,power), 1.0, 1. };
	}
	static Material green_diffuse() noexcept {
		constexpr Color c = Color(0.1, 0.4, 0.1);
		return { c, c, {}, 0.9 , 10. };
	}
	static Material red_shiny() noexcept { 
		return { Color(0.2,0.01,0.01), Color(0.8, 0.1, 0.1), {}, 0.1, 100. };
		}
	static Material mirror() noexcept { 
		return { {}, Color(0.999,0.999,0.999), {}, 0.0, 10000. };
		}
	static Material grey_diffuse(double greyVal = 0.2) noexcept { 
		return { Color(greyVal, greyVal, greyVal), {}, {}, 1., 1. };
	}
};

static_assert(std::is_copy_constructible_v<Material>, "Material not copy constructible!");
static_assert(std::is_copy_assignable_v<Material>, "Material not copy assignable!");