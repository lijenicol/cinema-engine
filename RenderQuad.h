#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

class RenderQuad
{
public:
	RenderQuad();
	~RenderQuad();
	void draw();
	GLuint vao, vbo;
};

