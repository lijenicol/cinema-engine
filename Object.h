#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "Material.h"

class Object
{
protected:
	glm::mat4 model;
	glm::mat4 rotation;
	glm::vec3 color;
	Material *material;
public:
	glm::mat4 getModel() { return model; }
	glm::vec3 getColor() { return color; }
	Material getMaterial(){ return *material; }

	virtual void draw() = 0;
	virtual void update() = 0;
};

#endif

