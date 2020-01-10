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

class Terrain : public Node
{
private:
	//geometry data
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::uvec3> triangles;
	std::vector<glm::uvec3> normalIndices;
	std::vector<glm::uvec3> texCoordIndices;
	glm::mat4 lastView;
	glm::mat4 prevModel;
	int width, length;

	//Initializes the terrain vertices by looking up a heightmap
	void initVertices(std::string heightMapLoc, int width, int length, float maxHeight);

	//Initializes the normals in the terrain at each vertex
	void initNormals(int width, int length);

	//Calculates the normal at a single vertex
	glm::vec3 calculateNormal(int x, int z);
	
	//Initializes the Index Buffer such that the model can be rendered using triangle strips
	void initIndexBuffer(int width, int length);

	//Loads a texture for the terrain and initializes its properties
	GLuint loadTexture(std::string filename);

public:
	Terrain(std::string heightMapLoc, std::string blendTextureLoc, int width, int length, float maxHeight);
	~Terrain();

	//Draws the terrain
	void draw(glm::mat4 C, glm::mat4 lastM);

	//Performs any updates necessary on the terrain
	void update(float deltaTime);

	std::vector<unsigned int> indices;
	GLuint vao, vbo, vbo_n, vbo_t, ebo;
	GLuint textureID, blendMapID, rTextureID, gTextureID, bTextureID;
};
