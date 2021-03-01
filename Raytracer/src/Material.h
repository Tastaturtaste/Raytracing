#pragma once
#include "Color.h"
#include "Constants.h"

struct Material
{
	Color diffuse_color{};
	Color light_color{};
	double prob_diffuse{1.0};
	double reflection_cone_radians{0.0};

	constexpr Material() = default;
	constexpr Material(Color diff_color, Color light_color, double prob_diffuse, double reflection_cone_radians) noexcept
		: diffuse_color(diff_color), light_color(light_color), prob_diffuse(prob_diffuse), reflection_cone_radians(reflection_cone_radians) {}

	static Material white_light(double power) noexcept { 
		return { {}, Color(power,power,power), 1.0, constants::pi }; 
	}
	static Material green_diffuse() noexcept {
		return { Color(0.1,0.4,0.1), {}, 0.9, constants::pi / 2.};
	}
	static Material red_shiny() noexcept { 
		return { Color(0.8,0.2,0.2), {}, 0.1, constants::pi / 8. };
		}
	static Material mirror() noexcept { 
		return { Color(0.999,0.999,0.999), {}, 0.0, 0.0 };
		}
	static Material grey_diffuse(double greyVal = 0.2) noexcept { 
		return { Color(greyVal, greyVal, greyVal), {}, 0.9, constants::pi / 2.};
	}
};

static_assert(std::is_copy_constructible_v<Material>, "Material not copy constructible!");
static_assert(std::is_copy_assignable_v<Material>, "Material not copy assignable!");