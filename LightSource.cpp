#include "LightSource.h"

LightSource::LightSource(glm::vec3 pos, glm::vec3 col, float attenuation, int directional, glm::vec3 direction) {
	this->pos = pos;
	this->col = col;
	this->attenuation = attenuation;
	this->directional = directional;
	this->direction = direction;
}

LightSource::~LightSource(){}
