#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"
#include "CubicBezier.h"
#include "Transform.h"
#include "PointSphere.h"
#include <vector>


class BezierLoop : public Node
{
private:
	std::vector<glm::vec3> bezierPoints;
	std::vector<CubicBezier*> beziers;
	glm::vec3 selectedPoint;
	int selectedIndex;
	int bezierLoopSize;
	Transform* pointSphereRoot;
	std::vector<PointSphere*> pointSpheres;
	std::vector<Transform*> pointSphereTransforms;
	std::vector<glm::vec3> handleLinePoints;
	GLuint vao, vbo;

public:
	BezierLoop(int n);
	~BezierLoop();
	std::vector<CubicBezier*> generateBezierLoop(int n);

	void draw(glm::mat4 C, glm::mat4 lastM);
	void update(float deltaTime);
	void moveSelection(int direction);
	void moveSelectedPoint(float dx, float dy, float dz);
	glm::vec3 getPointOnLoop(float t);
	void calculateHandlePoints();
};

