#pragma once
#include "Geometry.h"

class PointSphere : public Geometry
{
private:
	glm::vec3 defaultColor;
	glm::vec3 color;
	bool isSelected;
public:
	PointSphere(glm::vec3 color);
	void draw(glm::mat4 C);
	void selectionStatus(bool state);
};

