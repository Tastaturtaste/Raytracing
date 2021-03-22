#include "Scene.h"
#include <unordered_map>

namespace Scenes {

	Scene basic_scene()
	{
		constexpr double greyval = 0.2;
		std::vector<std::unique_ptr<Material>> materials{};
		materials.push_back(std::make_unique<Material>(Material::grey_diffuse(greyval)));
		materials.push_back(std::make_unique<Material>(Material::white_light(50.0)));
		materials.push_back(std::make_unique<Material>(Material::green_diffuse()));
		materials.push_back(std::make_unique<Material>(Material::red_shiny()));
		materials.push_back(std::make_unique<Material>(Material::mirror()));

		std::unordered_map<std::string, Material*> mats{
			{"grey_diffuse", materials.at(0).get()},
			{"white_light", materials.at(1).get()},
			{"green_diffuse", materials.at(2).get()},
			{"red_shiny", materials.at(3).get()},
			{"mirror", materials.at(4).get()}
		};
		constexpr double bigR = 1e5;
		std::vector<Geometry> geometries{
		Sphere(glm::dvec3(-bigR-1.,	1.,	1.),		bigR, mats.at("grey_diffuse")),		// left wall
		Sphere(glm::dvec3( bigR+1.,	1.,	1.),		bigR, mats.at("grey_diffuse")),		// right wall
		Sphere(glm::dvec3( 0., bigR+2.,	1.),		bigR, mats.at("grey_diffuse")),		// ceiling
		Sphere(glm::dvec3( 0.,-bigR	,	1.),		bigR, mats.at("grey_diffuse")),		// floor
		Sphere(glm::dvec3( 0., 1.	,-bigR-1),		bigR, mats.at("grey_diffuse")),		// backwall
		Sphere(glm::dvec3( 0., 1.,	bigR + 3.05),	bigR, mats.at("grey_diffuse")),		// frontwall
		Sphere(glm::dvec3( 0.,	2.,		2.),		0.4, mats.at("white_light")),		// lightsource
		Sphere(glm::dvec3(-.4,	0.4,	0.),		0.4, mats.at("green_diffuse")),		// green sphere
		Sphere(glm::dvec3( .35,	0.3,	0.),		0.3, mats.at("red_shiny")),			// shine red sphere
		Sphere(glm::dvec3(0.,	1.,	-.5),			0.3, mats.at("mirror"))				// mirror sphere
		};
		return Scene(std::move(geometries),std::move(materials));
	}

	//Scene basic_scene_with_triangle()
	//{
	//	constexpr double greyval = 0.2;
	//	std::unordered_map<std::string, std::unique_ptr<Material>> mats{
	//		{"grey_diffuse", std::move(Material::grey_diffuse(greyval))},
	//		{"white_light", std::move(Material::white_light(50.0))},
	//		{"green_diffuse", std::move(Material::green_diffuse())},
	//		{"red_shiny", std::move(Material::red_shiny())}
	//	};
	//	return Scene({
	//	Sphere(glm::dvec3(1e5,		0.5,		0.5),		1e5, mats.at("grey_diffuse"))),		// left wall
	//	Sphere(glm::dvec3(-1e5 + 1.0,	0.5,		0.5),		1e5, mats.at("grey_diffuse"))),		// right wall
	//	Sphere(glm::dvec3(0.5,		0.5,		-1e5 + 1),	1e5, mats.at("grey_diffuse"))),		// ceiling
	//	Sphere(glm::dvec3(0.5,		0.5,		1e5),		1e5, mats.at("grey_diffuse"))),		// floor
	//	Sphere(glm::dvec3(0.5,		-1e5 + 1,		0.5),		1e5, mats.at("grey_diffuse"))),		// backwall
	//	Sphere(glm::dvec3(0.5,		1e5,		0.5),		1e5, mats.at("grey_diffuse"))),		// frontwall
	//	Sphere(glm::dvec3(0.4,		0.2,		0.85),		0.1, mats.at("white_light")),	// lightsource
	//	Sphere(glm::dvec3(0.7,		0.5,		0.2),		0.2, mats.at("green_diffuse")),	// green sphere
	//	Sphere(glm::dvec3(0.2,		0.7,		0.2),		0.2, mats.at("red_shiny")),			// shine red sphere
	//	Triangle({glm::dvec3(0.5,0.7,0.9),glm::dvec3(0.9,0.7,0.1),glm::dvec3(0.1,0.7,0.1)}, mats.at("red_shiny"))
	//		});
	//}
} // Scenes