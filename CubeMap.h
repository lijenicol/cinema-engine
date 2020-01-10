#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "Object.h"

class CubeMap : public Object
{
private:
	GLuint vao;
	GLuint vbo;
	GLuint tex;
public:
	CubeMap(std::vector<std::string> faces);
	~CubeMap();

	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int getTexId();
	void draw();
	void update();
};

