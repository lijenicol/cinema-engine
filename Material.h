#pragma once
#include <glm/glm.hpp>
#include <string>

class Material {
public:
	Material(std::string textureLocation, glm::vec3 difCol, float difAmount, glm::vec3 specCol, float specAmount, float specCoefficient, float ambientAmount, float glowIntensity);
	glm::vec3 difCol;
	float difAmount;
	glm::vec3 specCol;
	float specAmount;
	float specCoefficient;
	float ambientAmount;
	std::string textureLocation;
	float glowIntensity;
};