#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"
#include <vector>
class CubicBezier : public Node
{
private:
	std::vector<glm::vec3> points;
	glm::vec3 p0, p1, p2, p3;
	GLuint vao, vbo;
	glm::mat4 modelView;
	glm::mat4 bezierMatrix;
public:
	CubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	~CubicBezier();

	void draw(glm::mat4 C, glm::mat4 lastM);
	void update(float deltaTime);
	void movePoints(glm::vec3 dp0, glm::vec3 dp1, glm::vec3 dp2, glm::vec3 dp3);
	std::vector<glm::vec3> getPoints();
	glm::mat4 computeBezierMatrix();
	void computeLineSegmentPoints();
	glm::vec3 getPointOnCurve(float t);
};

