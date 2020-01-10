#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif


class FrameBuffer
{
public:
	FrameBuffer(int width, int height, int nTex);
	~FrameBuffer();
	GLuint fbo, tex[];
};

