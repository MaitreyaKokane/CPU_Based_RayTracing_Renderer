#pragma once
#include <glm/glm.hpp>
#include "Vector"

struct Material
{
	glm::vec3 Albedo{ 0.5f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	glm::vec3 EmissionColor{ 0.0f };
	float EmissionStrength = 0.0f;
	bool EmissionEnabled = true;

	glm::vec3 GetEmission() const {
		return EmissionEnabled ? EmissionColor * EmissionStrength : glm::vec3(0.0f);
	}

};

struct Sphere
{
	glm::vec3 Position{ 0 };
	float Radius = 0.5f;
	
	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Sphere;
	std::vector<Material> Material;	
};