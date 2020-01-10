#include "Particle.h"

Particle::Particle(Transform* transform, glm::vec3 velocity, float lifetime) {
	this->transform = transform;
	this->velocity = velocity;
	this->lifetime = lifetime;
	this->lifeLeft = lifetime;
}

Particle::~Particle() {
	delete transform;
}

void Particle::update(float deltaTime) {
	transform->M = glm::translate(transform->M, velocity * deltaTime);
	lifeLeft -= deltaTime;
}