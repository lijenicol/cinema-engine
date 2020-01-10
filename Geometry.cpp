#include "Geometry.h"
#include "Window.h"

#include "stb_image.h"

const float SCALE_FACTOR = 9.0f;

Geometry::Geometry(std::string filename, Material *m) {
	material = m;
	init(filename);
	textureID = loadTexture(m->textureLocation);
	prevModel = glm::mat4(1);
	lastView = glm::mat4(1);
}

void Geometry::init(std::string filename) {
	//
	//READ IN GEOMETRY DATA FROM FILE
	//
	std::vector<glm::vec3> inputNormals;
	std::vector<glm::vec3> inputPoints;
	std::vector<glm::vec2> inputTexCoords;
	std::ifstream objStream(filename);
	std::string line;
	if (objStream.is_open()) {
		//go through every line of the obj file
		while (getline(objStream, line)) {
			std::stringstream ss;
			ss << line;

			//check for vertex label
			std::string label;
			ss >> label;
			if (label == "v") {
				//construct a point from the line
				glm::vec3 point;
				ss >> point.x;
				ss >> point.y;
				ss >> point.z;

				//push the point to the point list
				inputPoints.push_back(point);
			}
			if (label == "vn") {
				glm::vec3 normal;
				ss >> normal.x;
				ss >> normal.y;
				ss >> normal.z;

				//push the normal to the normals list
				inputNormals.push_back(normal);
			}
			if (label == "vt") {
				glm::vec2 texCoord;
				ss >> texCoord.x;
				ss >> texCoord.y;

				//push the coord to the texcoords list
				inputTexCoords.push_back(texCoord);
			}
			if (label == "f") {
				glm::uvec3 triangle;
				glm::uvec3 normalIndex;
				glm::uvec3 texCoordIndex;
				std::string s;
				int pos;
				std::string v;
				std::string u;

				//search face for / delimeter
				ss >> s;
				pos = s.find("/");
				v = s.substr(0,pos);
				triangle.x = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.x = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.x = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.y = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.y = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.y = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.z = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.z = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.z = std::stoi(s) - 1;

				triangles.push_back(triangle);
				normalIndices.push_back(normalIndex);
				texCoordIndices.push_back(texCoordIndex);
			}
		}
		objStream.close();
	}
	else {
		std::cout << "File cannot be read or does not exist: " << filename;
	}
	
	//reorder and duplicate indices so that indices line up (thanks normals)
	for (unsigned int i = 0; i < triangles.size(); i++) {
		points.push_back(inputPoints[triangles[i].x]);
		points.push_back(inputPoints[triangles[i].y]);
		points.push_back(inputPoints[triangles[i].z]);
		normals.push_back(inputNormals[normalIndices[i].x]);
		normals.push_back(inputNormals[normalIndices[i].y]);
		normals.push_back(inputNormals[normalIndices[i].z]);
		texCoords.push_back(inputTexCoords[texCoordIndices[i].x]);
		texCoords.push_back(inputTexCoords[texCoordIndices[i].y]);
		texCoords.push_back(inputTexCoords[texCoordIndices[i].z]);
		indices.push_back(i*3);
		indices.push_back(i*3+1);
		indices.push_back(i*3+2);
	}

	//
	//TRANSFORM POINTS SO THAT MODEL IS CENTERED
	//

	//traverse points and calculate the middle between them
	float lowestX = points[0].x;
	float highestX = points[0].x;
	float lowestY = points[0].y;
	float highestY = points[0].y;
	float lowestZ = points[0].z;
	float highestZ = points[0].z;

	for (int i = 0; i < points.size(); i++) {
		//X points
		if (points[i].x < lowestX) {
			lowestX = points[i].x;
		}
		else if (points[i].x > highestX) {
			highestX = points[i].x;
		}

		//Y points
		if (points[i].y < lowestY) {
			lowestY = points[i].y;
		}
		else if (points[i].y > highestY) {
			highestY = points[i].y;
		}

		//Z points
		if (points[i].z < lowestZ) {
			lowestZ = points[i].z;
		}
		else if (points[i].z > highestZ) {
			highestZ = points[i].z;
		}
	}

	//calculate mid points
	float midX = (lowestX + highestX) / 2.0f;
	float midY = (lowestY + highestY) / 2.0f;
	float midZ = (lowestZ + highestZ) / 2.0f;

	//translate all the points and also calculate max distance 
	float maxDist = 0;
	for (int i = 0; i < points.size(); i++) {
		points[i].x -= midX;
		points[i].y -= midY;
		points[i].z -= midZ;

		float dist = sqrt(pow(points[i].x, 2) + pow(points[i].y, 2) + pow(points[i].z, 2));
		if (dist > maxDist) {
			maxDist = dist;
		}
	}

	//scale all points to the max distance and scale factor accordingly
	//also push these final processed points into the original points vector
	for (int i = 0; i < points.size(); i++) {
		points[i].x = points[i].x / maxDist * SCALE_FACTOR;
		points[i].y = points[i].y / maxDist * SCALE_FACTOR;
		points[i].z = points[i].z / maxDist * SCALE_FACTOR;

		glm::vec3 pushPoint;
		pushPoint.x = points[i].x;
		pushPoint.y = points[i].y;
		pushPoint.z = points[i].z;
	}

	//
	//SETUP OPENGL BUFFERS HERE
	//

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
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

GLuint Geometry::loadTexture(std::string location) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(location.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D,
			0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Cubemap texture failed to load at path: " << location << std::endl;
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

Geometry::~Geometry() {
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_n);
	glDeleteBuffers(1, &vbo_t);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Geometry::draw(glm::mat4 C, glm::mat4 lastM)
{
	glUseProgram(Window::program);
	// Set model view matrix
	glUniformMatrix4fv(Window::modelLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(Window::prevModelLoc, 1, GL_FALSE, glm::value_ptr(lastM));
	glUniformMatrix4fv(Window::viewLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->view));
	glUniformMatrix4fv(Window::prevViewLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->lastFrameView));
	glUniformMatrix4fv(Window::projectionLoc, 1, GL_FALSE, glm::value_ptr(Window::activeScene->activeCamera->projection));
	
	//set material properties
	glUniform1f(glGetUniformLocation(Window::program, "glowIntensity"), material->glowIntensity);

	//bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw the model
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	prevModel = C;
	lastView = Window::activeScene->activeCamera->view;
}

void Geometry::update(float deltaTime) {}
