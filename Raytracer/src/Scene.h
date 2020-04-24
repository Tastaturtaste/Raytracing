#pragma once
#include <vector>
#include <array>
#include "Sphere.h"
#include "Material.h"

class Scene
{
	//static inline std::array materials{
	//	Material({ .5, .5, .5},{ .5, .5, .5},{ 20.0, 20.0, 20.0},0.5f),	// pure lightsource
	//	Material({0.0,0.8,0.0},{0.0,0.8,0.0},{0.0,0.0,0.0},0.9f),	// half green, mostly diffuse
	//	Material({0.8,0.0,0.0},{0.8,0.0,0.0},{0.0,0.0,0.0},0.1f),	// half green, half diffuse
	//	Material({0.8,0.8,0.8},{0.8,0.8,0.8},{},1.0f)				// grey, full diffuse
	//	};
	std::vector<Sphere> spheres_;
public:
	Scene(const std::vector<Sphere>& spheres)
		: spheres_(spheres) {}

	[[nodiscard]] const auto& spheres() const noexcept { return spheres_; }

	void add_sphere(Sphere sphere) { spheres_.push_back(std::move(sphere)); }

	static Scene basic_scene() {
		return Scene({
		Sphere(Vec3(1e5,		0.5,	0.5),		1e5, Material::grey_diffuse()),		// left wall
		Sphere(Vec3(-1e5+1.0,	0.5,	0.5),		1e5, Material::grey_diffuse()),		// right wall
		Sphere(Vec3(0.5,		-1e5+1,	0.5),		1e5, Material::grey_diffuse()),		// ceiling
		Sphere(Vec3(0.5,		1e5,	0.5),		1e5, Material::grey_diffuse()),		// floor
		Sphere(Vec3(0.5,		0.5,	-1e5+1),	1e5, Material::grey_diffuse()),		// backwall
		Sphere(Vec3(0.5,		0.5,	1e5),		1e5, Material::grey_diffuse()),		// frontwall
		Sphere(Vec3(0.1,		0.85,	0.2),		0.1, Material::white_light(20.0)),	// lightsource
		Sphere(Vec3(0.7,		0.2,	0.5),		0.2, Material::green_diffuse()),	// green sphere
		Sphere(Vec3(0.2,		0.2,	0.7),		0.2, Material::red_shiny())			// shine red sphere
		});
	}
	static Scene hull_sphere() {
		return Scene({
		Sphere(Vec3(0.5,		0.5,	0.5),		2.0, Material::grey_diffuse()),		// wall
		Sphere(Vec3(0.1,		0.85,	-0.2),		0.1, Material::white_light(200.0)),	// lightsource
		Sphere(Vec3(0.7,		0.2,	0.5),		0.2, Material::green_diffuse()),	// green sphere
		Sphere(Vec3(0.2,		0.2,	0.7),		0.2, Material::red_shiny()),		// shine red sphere
		Sphere(Vec3(0.5,		0.5,	0.8),		0.2, Material::mirror())			// mirror sphere
			});
	}
};