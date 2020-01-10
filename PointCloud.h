#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Object.h"
#include "Material.h"

class PointCloud : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> originalPoints;
	std::vector<glm::vec3> normals;
	std::vector<glm::uvec3> triangles;
	GLuint vao, vbo, vbo_n, ebo;
	GLfloat pointSize;
	float localSpinCounter;
	bool randomAnimPlaying;
	float animationStartTime;
	double scale;
	float lerp(float p1, float p2, float t);
public:
	PointCloud(std::string objFilename, GLfloat pointSize, Material *material);
	~PointCloud();

	void draw();
	void update();

	std::vector<glm::vec3>* getPoints();
	float getLocalSpinCounter();
	void setLocalSpinCounter(float s);

	void updatePointSize(GLfloat size);
	void spin(float deg);
	void rotateAboutAxis(float deg, glm::vec3 axis);
	void randomizePoints();
	void setPoints(std::vector<glm::vec3>* p);
	void returnPointsToOriginal();
	void interpolatePoints(float t);
	void updateBuffer();
	void changeScale(double delta);
};

#endif

