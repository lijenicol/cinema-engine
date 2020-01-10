#pragma once
#include "Node.h"
#include <list>

class Transform : public Node
{
public:
	Transform(glm::mat4 M);
	~Transform();
	void draw(glm::mat4 C, glm::mat4 lastM);
	void update(float deltaTime);
	void addChild(Node *child);

	glm::mat4 M, prevM;
	std::list<Node*> children;
	void updateModel(glm::mat4 M);
};