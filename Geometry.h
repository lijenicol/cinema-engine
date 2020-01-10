#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"
#include "Material.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class Geometry : public Node
{
private:
	//geometry data
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::uvec3> triangles;
	std::vector<glm::uvec3> normalIndices;
	std::vector<glm::uvec3> texCoordIndices;
	glm::mat4 prevModel;
	glm::mat4 lastView;

	//material data
	Material* material;
	unsigned int textureID;

public:
	Geometry(std::string filename, Material *m);
	~Geometry();

	void draw(glm::mat4 C, glm::mat4 lastM);
	void update(float deltaTime);	
	void init(std::string filename);
	GLuint loadTexture(std::string location);

	std::vector<unsigned int> indices;
	GLuint vao, vbo, vbo_n, vbo_t, ebo;
};

