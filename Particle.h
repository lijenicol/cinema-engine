#pragma once
#include "Transform.h"
#include <glm/gtx/transform.hpp>

class Particle
{
public:
	Particle(Transform *transform, glm::vec3 velocity, float lifetime);
	~Particle();
	void update(float deltaTime);
	Transform* transform;
	glm::vec3 velocity;
	float lifeLeft, lifetime;
};

