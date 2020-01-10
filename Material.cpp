#include "Material.h"

Material::Material(std::string textureLocation, glm::vec3 difCol, float difAmount, glm::vec3 specCol, float specAmount, float specCoefficient, float ambientAmount, float glowIntensity) :
	difCol(difCol), difAmount(difAmount), specCol(specCol), specAmount(specAmount), specCoefficient(specCoefficient), ambientAmount(ambientAmount)
{
	this->textureLocation = textureLocation;
	this->glowIntensity = glowIntensity;
}