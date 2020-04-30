#include "Scene.h"
#include <glm\glm.hpp>


std::optional<IntersectionTrace> Scene::find_nearest(const Ray& r) const {
	std::optional<IntersectionTrace> nearest{};
	std::optional<IntersectionTrace> temp{};
	double min_dist = std::numeric_limits<double>::infinity();
	for (const Sphere& sphere : spheres_) {
		temp = sphere.intersect(r);
		if (temp.has_value() && temp->distance < min_dist) {
			min_dist = temp->distance;
			nearest = temp;
		}
	}
	return nearest;
}

Scene Scene::basic_scene()
{
	constexpr double greyval = 0.6;
	return Scene({
	Sphere(glm::dvec3(1e5,		0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// left wall
	Sphere(glm::dvec3(-1e5+1.0,	0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// right wall
	Sphere(glm::dvec3(0.5,		0.5,		-1e5+1),	1e5, Material::grey_diffuse(greyval)),		// ceiling
	Sphere(glm::dvec3(0.5,		0.5,		1e5),		1e5, Material::grey_diffuse(greyval)),		// floor
	Sphere(glm::dvec3(0.5,		-1e5+1,		0.5),		1e5, Material::grey_diffuse(greyval)),		// backwall
	Sphere(glm::dvec3(0.5,		1e5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// frontwall
	Sphere(glm::dvec3(0.4,		0.2,		0.85),		0.1, Material::white_light(20.0)),	// lightsource
	Sphere(glm::dvec3(0.7,		0.5,		0.2),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(glm::dvec3(0.2,		0.7,		0.2),		0.2, Material::red_shiny())			// shine red sphere
	});
}

Scene Scene::hull_sphere()
{
	constexpr double greyval = 0.6;
	return Scene({
	Sphere(glm::dvec3(0.5,	 0.5,	0.5),		2.0, Material::grey_diffuse(greyval)),		// wall
	Sphere(glm::dvec3(0.1,	-0.2,	0.85),		0.1, Material::white_light(100.0)),	// lightsource
	Sphere(glm::dvec3(0.7,	 0.5,	0.2),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(glm::dvec3(0.2,	 0.7,	0.2),		0.2, Material::red_shiny()),		// shine red sphere
	Sphere(glm::dvec3(0.5,	 0.8,	0.5),		0.2, Material::mirror())			// mirror sphere
		});
}

Scene Scene::light_distribution()
{
	constexpr double greyval = 0.6;
	return Scene({
	Sphere(glm::dvec3(0.5,	 0.5,	0.5),		2.0, Material::grey_diffuse(greyval)),		// wall
	Sphere(glm::dvec3(0.5,	 2.5,	0.5),		0.05, Material::green_diffuse()),	// middle backwall
	Sphere(glm::dvec3(0.5,	-0.2,	0.5),		0.1, Material::white_light(100.0)),	// lightsource
		});
}

Scene Scene::debug_scene()
{
	return Scene({
		Sphere(glm::dvec3(0.5,0.5,0.5),	0.1,	Material::white_light(20.0)),		// lightsource in the middle of scene
		Sphere(glm::dvec3(0.2,0.5,0.5),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere left
		Sphere(glm::dvec3(0.8,0.5,0.5),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere right
		Sphere(glm::dvec3(0.5,0.5,0.2),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere bottom
		Sphere(glm::dvec3(0.5,0.5,0.8),	0.1,	Material::grey_diffuse(0.6))			// gray sphere top
		});
}