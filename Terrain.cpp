#include "Terrain.h"
#include "stb_image.h"
#include "Window.h"

Terrain::Terrain(std::string heightMapLoc, std::string blendTextureLoc, int width, int length, float maxHeight) {
	this->width = width;
	this->length = length;

	prevModel = glm::mat4(1);
	lastView = glm::mat4(1);
	
	//calculate vertex positions
	initVertices(heightMapLoc, width, length, maxHeight);
	
	//calculate vertex normals
	initNormals(width, length);

	//initialize index buffer
	initIndexBuffer(width, length);

	//load texture
	textureID = loadTexture("textures/grass.jpg");
	blendMapID = loadTexture("textures/blendmap.jpg");
	rTextureID = loadTexture("textures/snow.jpg");
	gTextureID = loadTexture("textures/stone.jpg");

	//LOAD IN DATA TO GPU
	// Generate a vertex array (VAO) and a vertex buffer objects (VBO) + EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_n);
	glGenBuffers(1, &vbo_t);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//Send normal data to gpu
	glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//Send texture coord data to gpu
	glBindBuffer(GL_ARRAY_BUFFER, vbo_t);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	//Bind to the EBO 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//Send the information to the gpu
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

Terrain::~Terrain() {}

//The algorithm used here could be improved such that the terrain is smoother
void Terrain::initVertices(std::string heightMapLoc, int width, int length, float maxHeight) {
	//load in the height map : note that we only load in the first channel because that is all we need
	int heightMapWidth, heightMapHeight, nrChannels;
	unsigned short* heightMapData = stbi_load_16(heightMapLoc.c_str(), &heightMapWidth, &heightMapHeight, &nrChannels, 1);
	
	//if the heightmap exists then initialize terrain points from heightmap
	if (heightMapData)
	{
		//Then do the processing here
		float strideH = (heightMapWidth-1) / width;
		float strideV = (heightMapHeight-1) / length;
		for (int i = 0; i <= length; i++) {
			//Get pixel value and push to the list of heights
			for (int j = 0; j <= width; j++) {
				//Calculate the index to retrieve pixel value from
				int index_x = strideH * j;
				int index_y = strideV * i;

				//Retrieve pixel value
				unsigned short pixelVal = heightMapData[index_y * heightMapWidth + index_x + 1];
				
				//Calculate height value
				float height = (pixelVal - 32767) / (float)32768 * maxHeight;
				//float height = (pixelVal) / (float)65536 * maxHeight;

				//Push vertex to list
				glm::vec3 point((float)j / width, height, (float)i / length);
				points.push_back(glm::vec3(point));

				//While we are here also push texture coords
				glm::vec2 texCoord = glm::vec2(point.x, point.z);
				texCoords.push_back(texCoord);
			}
		}
		stbi_image_free(heightMapData);
	}
	
	//Otherwise initialize a default flat terrain
	else
	{
		//Initialize height array to all zeroes so that some terrain is created
		for (int i = 0; i <= length; i++) {
			for (int j = 0; j <= width; j++) {
				//Push vertex to list
				glm::vec3 point((float)j / width, 0, (float)i / length);
				points.push_back(glm::vec3(point));

				//While we are here also push texture coords
				glm::vec2 texCoord = glm::vec2(point.x, point.z);
				texCoords.push_back(texCoord);
			}
		}
		std::cout << "image failed to load at path: " << heightMapLoc << std::endl;
		std::cout << "Initializing default terrain" << std::endl;
		stbi_image_free(heightMapData);
	}
}

void Terrain::initNormals(int width, int length) {
	for (int i = 0; i <= length; i++) {
		for (int j = 0; j <= width; j++) {
			glm::vec3 normal = calculateNormal(j, i);
			normals.push_back(normal);
		}
	}
}

glm::vec3 Terrain::calculateNormal(int x, int z) {
	//Handle edge cases 
	if (x == 0) {
		x = 1;
	}
	if (z == 0) {
		z = 1;
	}
	if (x == width) {
		x--;
	}
	if (z == length) {
		z--;
	}
	
	//Get points of neighbours
	glm::vec3 hl = points.at((z) * (width+1) + (x-1));
	glm::vec3 hr = points.at((z) * (width + 1) + (x+1));
	glm::vec3 hd = points.at((z+1) * (width + 1) + (x));
	glm::vec3 hu = points.at((z-1) * (width + 1) + (x));

	//Calculate normal based upon cross product
	glm::vec3 n = glm::normalize(glm::cross(hr-hl, hu-hd));

	return n;
}

void Terrain::initIndexBuffer(int width, int length) {
	for (int i = 0; i <= length - 1; i++) {
		for (int j = 0; j <= width; j++) {
			int point = i * (width+1) + j;
			if (j == 0 && i > 0) {
				indices.push_back(point);
			}

			point = i * (width + 1) + j;
			indices.push_back(point);
			point = (i + 1) * (width + 1) + j;
			indices.push_back(point);

			if (j == width && i < length - 1) {
				indices.push_back(point);
			}
		}
	}
}

GLuint Terrain::loadTexture(std::string filename) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D,
			0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	//Setup texture attributes & unbind to finish
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void Terrain::draw(glm::mat4 C, glm::mat4 lastM) {
	//Use the deferred rendering shader
	glUseProgram(Window::program);
	
	// Set model view matrix
	glUniformMatrix4fv(Window::modelLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(Window::viewLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->view));
	glUniformMatrix4fv(Window::projectionLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->projection));
	glUniformMatrix4fv(Window::prevViewLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->lastFrameView));
	glUniformMatrix4fv(Window::prevModelLoc, 1, GL_FALSE, glm::value_ptr(lastM));
	
	//Set material properties
	glUniform1f(glGetUniformLocation(Window::program, "glowIntensity"), 0);
	
	//Setup texture units and bind textures
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTextureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, blendMapID);

	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw the model
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
	
	//Unbind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Update previous model and view matrices
	prevModel = C;
	lastView = Window::activeScene->activeCamera->view;
}


void Terrain::update(float deltaTime) {}