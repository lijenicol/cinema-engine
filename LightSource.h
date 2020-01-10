#pragma once
#include "Object.h"
#include <glm/glm.hpp>

class LightSource {
private:
	
public:
	LightSource(glm::vec3 pos, glm::vec3 col, float attenuation, int directional, glm::vec3 direction);
	~LightSource();
	glm::vec3 pos;
	glm::vec3 col;
	float attenuation;
	int directional;
	glm::vec3 direction;
};