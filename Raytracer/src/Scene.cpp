#include "Scene.h"


std::optional<IntersectionTrace> Scene::find_nearest(const Ray& r) const noexcept {
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
	return Scene({
	Sphere(Vec3(1e5,		0.5,	0.5),		1e5, Material::grey_diffuse()),		// left wall
	Sphere(Vec3(-1e5+1.0,	0.5,	0.5),		1e5, Material::grey_diffuse()),		// right wall
	Sphere(Vec3(0.5,		-1e5+1,	0.5),		1e5, Material::grey_diffuse()),		// ceiling
	Sphere(Vec3(0.5,		1e5,	0.5),		1e5, Material::grey_diffuse()),		// floor
	Sphere(Vec3(0.5,		0.5,	-1e5+1),	1e5, Material::grey_diffuse()),		// backwall
	Sphere(Vec3(0.5,		0.5,	1e5),		1e5, Material::grey_diffuse()),		// frontwall
	Sphere(Vec3(0.2,		0.85,	0.2),		0.1, Material::white_light(20.0)),	// lightsource
	Sphere(Vec3(0.7,		0.2,	0.5),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(Vec3(0.2,		0.2,	0.7),		0.2, Material::red_shiny())			// shine red sphere
	});
}

Scene Scene::hull_sphere()
{
	return Scene({
	Sphere(Vec3(0.5,		0.5,	0.5),		2.0, Material::grey_diffuse()),		// wall
	Sphere(Vec3(0.1,		0.85,	-0.2),		0.1, Material::white_light(100.0)),	// lightsource
	Sphere(Vec3(0.7,		0.2,	0.5),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(Vec3(0.2,		0.2,	0.7),		0.2, Material::red_shiny()),		// shine red sphere
	Sphere(Vec3(0.5,		0.5,	0.8),		0.2, Material::mirror())			// mirror sphere
		});
}