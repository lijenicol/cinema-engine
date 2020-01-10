#include "PointCloud.h"
#include "Window.h"
#include <math.h>

const float SCALE_FACTOR = 9.0f;
const float ANIMATION_LENGTH = 100.0f;

PointCloud::PointCloud(std::string objFilename, GLfloat pointSize, Material *m) 
	: pointSize(pointSize)
{
	randomAnimPlaying = false;
	animationStartTime = 0;
	localSpinCounter = 0.0f;
	material = m;
	
	//set default behaviour to load points of a cube
	if (objFilename == "foo") {
		points =
		{
			glm::vec3(-2.5, 2.5, 2.5),
			glm::vec3(-2.5, -2.5, 2.5),
			glm::vec3(2.5, -2.5, 2.5),
			glm::vec3(2.5, 2.5, 2.5),
			glm::vec3(-2.5, 2.5, -2.5),
			glm::vec3(-2.5, -2.5, -2.5),
			glm::vec3(2.5, -2.5, -2.5),
			glm::vec3(2.5, 2.5, -2.5)
		};
	}

	//else load points from the specified obj
	else {
		std::ifstream objStream(objFilename);
		std::string line;
		if (objStream.is_open()) {
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
					points.push_back(point);
				}
				if (label == "vn") {
					glm::vec3 normal;
					ss >> normal.x;
					ss >> normal.y;
					ss >> normal.z;

					//push the normal to the normals list
					normals.push_back(normal);
				}
				if (label == "f") {
					//read the vertices for the triangles
					std::string s;
					std::stringstream vx;
					std::stringstream vy;
					std::stringstream vz;
					std::string v;
					glm::uvec3 triangle;
					
					ss >> s;
					vx << s;
					std::getline(vx, v, '/');
					//std::cout << v << " ";
					triangle.x = std::stoi(v) - 1;

					ss >> s;
					vy << s;
					std::getline(vy, v, '/');
					//std::cout << v << " ";
					triangle.y = std::stoi(v) - 1;

					ss >> s;
					vz << s;
					std::getline(vz, v, '/');
					//std::cout << v << "\n";
					triangle.z = std::stoi(v) - 1;

					triangles.push_back(triangle);
					//std::cout << triangle.x << " " << triangle.y << " " << triangle.z << "\n";
				}
			}
			objStream.close();
		}
		else {
			std::cout << "File cannot be read or does not exist: " << objFilename;
		}
	}

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

		//calculate max distance
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

		float dist = sqrt(pow(points[i].x,2) + pow(points[i].y, 2) + pow(points[i].z, 2));
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
		originalPoints.push_back(pushPoint);
	}

	// Set the model and rotation matrix to an identity matrix. 
	model = glm::mat4(1);
	rotation = glm::mat4(1);
	// Set the color. 
	color = glm::vec3(1, 1, 1);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO) + EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_n);
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
	
	//Bind to the EBO 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//Send the information to the gpu
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

PointCloud::~PointCloud() 
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void PointCloud::draw()
{
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw the model
	glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PointCloud::update()
{
	
}

float PointCloud::getLocalSpinCounter() {
	return localSpinCounter;
}

void PointCloud::setLocalSpinCounter(float s) {
	localSpinCounter = s;
}

std::vector<glm::vec3>* PointCloud::getPoints() {
	return &originalPoints;
}

void PointCloud::updatePointSize(GLfloat size) 
{
	pointSize = size;
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}


void PointCloud::rotateAboutAxis(float deg, glm::vec3 axis) {
	model = Window::rotateAboutAxis(deg, axis, model);
}

void PointCloud::randomizePoints() {
	randomAnimPlaying = true;
	animationStartTime = glfwGetTime();
	//randomize points depending on the scale factor
	for (int i = 0; i < points.size(); i++) {
		float r = SCALE_FACTOR * 2 * rand() / (float)RAND_MAX - SCALE_FACTOR;
		points[i].x = r;
		r = SCALE_FACTOR * 2 * rand() / (float)RAND_MAX - SCALE_FACTOR;
		points[i].y = r;
		r = SCALE_FACTOR * 2 * rand() / (float)RAND_MAX - SCALE_FACTOR;
		points[i].z = r;
	}
}

void PointCloud::setPoints(std::vector<glm::vec3>* p) {
	randomAnimPlaying = true;
	animationStartTime = glfwGetTime();
	for (int i = 0; i < points.size(); i++) {
		points[i] = p->at(i);
	}
}

void PointCloud::returnPointsToOriginal() {
	for (int i = 0; i < points.size(); i++) {
		points[i] = originalPoints[i];
	}
}

//Interpolates the points based on a smooth cosine function
void PointCloud::interpolatePoints(float t) {
	for (int i = 0; i < points.size(); i++) {
		//another method is to use a cosine smooth interpolation function
		//COSINE SMOOTH
		/*
		points[i].x = lerp(points[i].x, originalPoints[i].x, -(cos(glm::pi<float>() * t)/2) + 0.5f);
		points[i].y = lerp(points[i].y, originalPoints[i].y, -(cos(glm::pi<float>() * t) / 2) + 0.5f);
		points[i].z = lerp(points[i].z, originalPoints[i].z, -(cos(glm::pi<float>() * t) / 2) + 0.5f);
		*/

		//DECELERATION SMOOTH
		points[i].x = lerp(points[i].x, originalPoints[i].x, 1 - ((1 - t) * (1 - t)));
		points[i].y = lerp(points[i].y, originalPoints[i].y, 1 - ((1 - t) * (1 - t)));
		points[i].z = lerp(points[i].z, originalPoints[i].z, 1 - ((1 - t) * (1 - t)));
	}
}

float PointCloud::lerp(float p1, float p2, float t) {
	return p1 + t * (p2 - p1);
}

void PointCloud::updateBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	float* loc = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (loc) {
		//edit the points here
		for (int i = 0; i < points.size() * 3; i+=3) {
			loc[i] = points[i/3].x;
			loc[i+1] = points[i/3].y;
			loc[i+2] = points[i/3].z;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointCloud::changeScale(double delta) {
	scale = 1 - delta;
	model = glm::scale(model, glm::vec3(scale, scale, scale));
}
