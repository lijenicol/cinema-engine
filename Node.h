#pragma once
#include "glm/glm.hpp"

class Node
{
public:
	virtual void draw(glm::mat4 C, glm::mat4 lastM)=0;
	virtual void update(float deltaTime) = 0;
};

