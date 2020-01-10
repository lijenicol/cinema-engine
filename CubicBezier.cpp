#include "CubicBezier.h"
#include "Window.h"
#define LINE_COUNT 200

CubicBezier::CubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : p0(p0), p1(p1), p2(p2), p3(p3) {
	//compute bezier matrix for easy multiplication
	bezierMatrix = computeBezierMatrix();

	//compute line segments
	computeLineSegmentPoints();

	//SETUP OPENGL BUFFERS HERE
	//

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO) + EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

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

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

CubicBezier::~CubicBezier() {
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void CubicBezier::draw(glm::mat4 C, glm::mat4 lastM) {
	// Set model view matrix
	glUseProgram(Window::bezierShaderProgram);
	glm::mat4 i = glm::mat4(1);
	glUniformMatrix4fv(Window::bezierModelLoc, 1, GL_FALSE, glm::value_ptr(i));
	glUniformMatrix4fv(Window::bezierViewLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(Window::bezierProjectionLoc, 1, GL_FALSE, glm::value_ptr(Window::projection));
	glUniform3f(Window::bezierColorLoc, 1, 1, 1);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Draw the line segments
	glLineWidth(2.5);
	glDrawArrays(GL_LINES, 0, points.size());
	
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void CubicBezier::update(float deltaTime) {

}

void CubicBezier::movePoints(glm::vec3 dp0, glm::vec3 dp1, glm::vec3 dp2, glm::vec3 dp3) {
	p0 += dp0;
	p1 += dp1;
	p2 += dp2;
	p3 += dp3;

	//RECALCULATE ALL GRAPHICS DATA NECESSARY
	//compute bezier matrix for easy multiplication
	bezierMatrix = computeBezierMatrix();
	//compute line segments
	computeLineSegmentPoints();
	
	//SUBMIT CHANGES TO GPU
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::mat4 CubicBezier::computeBezierMatrix() {
	glm::vec4 a = -glm::vec4(p0, 0) + 3.0f * glm::vec4(p1, 0) - 3.0f * glm::vec4(p2, 0) + glm::vec4(p3, 0);
	glm::vec4 b = 3.0f * glm::vec4(p0, 0) - 6.0f * glm::vec4(p1, 0) + 3.0f * glm::vec4(p2, 0);
	glm::vec4 c = -3.0f * glm::vec4(p0, 0) + 3.0f * glm::vec4(p1, 0);
	glm::vec4 d = glm::vec4(p0, 0);
	return glm::mat4(a, b, c, d);
}

void CubicBezier::computeLineSegmentPoints() {
	points.clear();
	for (int i = 0; i < LINE_COUNT; i++) {
		//point 1
		glm::vec4 timeMatrix = glm::vec4(glm::pow((float)i / LINE_COUNT, 3), glm::pow((float)i / LINE_COUNT, 2), (float)i / LINE_COUNT, 1);
		glm::vec3 point = bezierMatrix * timeMatrix;
		points.push_back(point);
		
		//point 2
		glm::vec4 timeMatrix2 = glm::vec4(glm::pow((float)(i+1) / LINE_COUNT, 3), glm::pow((float)(i + 1) / LINE_COUNT, 2), (float)(i + 1) / LINE_COUNT, 1);
		glm::vec3 point2 = bezierMatrix * timeMatrix2;
		points.push_back(point2);

	}
}

glm::vec3 CubicBezier::getPointOnCurve(float t) {
	glm::vec4 timeMatrix = glm::vec4(glm::pow((float)t, 3), glm::pow((float)t, 2), (float)t, 1);
	glm::vec3 point = bezierMatrix * timeMatrix;
	return point;
}

std::vector<glm::vec3> CubicBezier::getPoints() {
	std::vector<glm::vec3> returnPoints;
	returnPoints.push_back(p0);
	returnPoints.push_back(p1);
	returnPoints.push_back(p2);
	returnPoints.push_back(p3);
	return returnPoints;
}