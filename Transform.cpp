#include "Transform.h"
#include "Window.h"

Transform::Transform(glm::mat4 M) : M(M) {
	prevM = M;
}

Transform::~Transform() {
	//delete children
	std::list<Node*>::iterator it;	//research iterators
	for (it = children.begin(); it != children.end(); ++it) {
		Node* child = *it;
		delete child;
	}
}

void Transform::update(float deltaTime) {}

//recursive draw function
void Transform::draw(glm::mat4 C, glm::mat4 lastM) {	
	std::list<Node*>::iterator it;	//research iterators
	for (it = children.begin(); it != children.end(); ++it) {
		Node* child = *it;
		child->draw(C * M, C * prevM);
	}
	prevM = M;
}

void Transform::addChild(Node* child) {
	children.push_back(child);
}

void Transform::updateModel(glm::mat4 M) {
	prevM = this->M;
	this->M = M;
}