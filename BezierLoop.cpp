#include "BezierLoop.h"
#include "Window.h"
#include "Transform.h"
#include "Geometry.h"
#include "PointSphere.h"
#include <glm/gtc/matrix_transform.hpp>

BezierLoop::BezierLoop(int n) : bezierLoopSize(n){
	beziers = generateBezierLoop(n);
	selectedIndex = 0;
	selectedPoint = bezierPoints[selectedIndex];

	//generate point sphere geometry
	pointSphereRoot = new Transform(glm::mat4(1));
	for (int i = 0; i < bezierPoints.size(); i++) {
		glm::vec3 point = bezierPoints[i];
		glm::mat4 pointSpherePosition = glm::translate(glm::mat4(1), point);
		glm::mat4 pointSphereScale = glm::scale(glm::mat4(1), glm::vec3(0.003,0.003,0.003));
		Transform* pointSphereTransform = new Transform(pointSpherePosition * pointSphereScale);
		pointSphereTransforms.push_back(pointSphereTransform);
		PointSphere* pointSphere;
		if (i % 3 == 0) {
			pointSphere = new PointSphere(glm::vec3(0, 1, 0));
		}
		else {
			pointSphere = new PointSphere(glm::vec3(1, 0, 0));
		}
		pointSpheres.push_back(pointSphere);

		//attach to point sphere root
		pointSphereRoot->addChild(pointSphereTransform);
		pointSphereTransform->addChild(pointSphere);
	}

	pointSpheres[selectedIndex]->selectionStatus(true);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO) 
	//for the line handles
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	calculateHandlePoints();
}

BezierLoop::~BezierLoop() {}


void BezierLoop::draw(glm::mat4 C, glm::mat4 prevM) {
	//draw beziers
	for (int i = 0; i < bezierLoopSize; i++) {
		beziers[i]->draw(C, prevM);
	}

	//render the handle lines
	glUseProgram(Window::bezierShaderProgram);
	glm::mat4 i = glm::mat4(1);
	glUniformMatrix4fv(Window::bezierModelLoc, 1, GL_FALSE, glm::value_ptr(i));
	glUniformMatrix4fv(Window::bezierViewLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(Window::bezierProjectionLoc, 1, GL_FALSE, glm::value_ptr(Window::projection));
	glUniform3f(Window::bezierColorLoc, 0, 1, 1);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Draw the line segments
	glLineWidth(2.5);
	glDrawArrays(GL_LINES, 0, handleLinePoints.size());

	// Unbind from the VAO.
	glBindVertexArray(0);

	//draw the spheres
	pointSphereRoot->draw(C, prevM);
}

void BezierLoop::update(float deltaTime) {
	for (int i = 0; i < bezierLoopSize; i++) {
		beziers[i]->update(deltaTime);
	}
}

void BezierLoop::moveSelection(int direction) {
	pointSpheres[selectedIndex]->selectionStatus(false);
	selectedIndex += direction;
	//this handles a weird modulus glitch (dont know what causes it)
	if (selectedIndex == -1) {
		selectedIndex = bezierPoints.size() - 1;
	}
	selectedIndex = selectedIndex % bezierPoints.size();
	selectedPoint = bezierPoints[selectedIndex];
	pointSpheres[selectedIndex]->selectionStatus(true);
}

void BezierLoop::moveSelectedPoint(float dx, float dy, float dz) {
	//move point
	selectedPoint += glm::vec3(dx, dy, dz);
	//move point sphere (would have to move transform)
	//update graphics memory
	//need to update bezier curve
	int selectedBezier = selectedIndex / 3; // divide 3 because of cubic bezier
	glm::vec3 vectorDelta = glm::vec3(dx, dy, dz);
	glm::vec3 nullVector = glm::vec3(0,0,0);
	//move endpoints of beziers
	if (selectedIndex % 3 == 0) {
		//update beziers
		beziers[selectedBezier]->movePoints(vectorDelta, nullVector, nullVector, nullVector);
		beziers[(selectedBezier - 1) % beziers.size()]->movePoints(nullVector, nullVector, nullVector, vectorDelta);
		beziers[selectedBezier]->movePoints(nullVector, vectorDelta, nullVector, nullVector);
		beziers[(selectedBezier - 1) % beziers.size()]->movePoints(nullVector, nullVector, vectorDelta, nullVector);
		//move geometry
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1), vectorDelta);
		pointSphereTransforms[selectedIndex]->M = translationMatrix * pointSphereTransforms[selectedIndex]->M;
		pointSphereTransforms[(selectedIndex - 1 + bezierPoints.size()) % bezierPoints.size()]->M = translationMatrix * pointSphereTransforms[(selectedIndex - 1 + bezierPoints.size()) % bezierPoints.size()]->M;
		pointSphereTransforms[(selectedIndex + 1 + bezierPoints.size()) % bezierPoints.size()]->M = translationMatrix * pointSphereTransforms[(selectedIndex + 1 + bezierPoints.size()) % bezierPoints.size()]->M;
	}
	if (selectedIndex % 3 == 1) {
		//move first approximating point and last approximating point of last bezier
		beziers[selectedBezier]->movePoints(nullVector, vectorDelta, nullVector, nullVector);
		beziers[(selectedBezier - 1) % beziers.size()]->movePoints(nullVector, nullVector, -vectorDelta, nullVector);

		//move geometry
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1), vectorDelta);
		pointSphereTransforms[selectedIndex]->M = translationMatrix * pointSphereTransforms[selectedIndex]->M;
		translationMatrix = glm::translate(glm::mat4(1), -vectorDelta);
		pointSphereTransforms[(selectedIndex - 2 + bezierPoints.size()) % bezierPoints.size()]->M = translationMatrix * pointSphereTransforms[(selectedIndex - 2 + bezierPoints.size()) % bezierPoints.size()]->M;
	}
	if (selectedIndex % 3 == 2) {
		//move last approximating point and first point of next bezier
		beziers[selectedBezier]->movePoints(nullVector, nullVector, vectorDelta, nullVector);
		beziers[(selectedBezier + 1) % beziers.size()]->movePoints(nullVector, -vectorDelta, nullVector, nullVector);

		//move geometry
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1), vectorDelta);
		pointSphereTransforms[selectedIndex]->M = translationMatrix * pointSphereTransforms[selectedIndex]->M;
		translationMatrix = glm::translate(glm::mat4(1), -vectorDelta);
		pointSphereTransforms[(selectedIndex + 2 + bezierPoints.size()) % bezierPoints.size()]->M = translationMatrix * pointSphereTransforms[(selectedIndex + 2 + bezierPoints.size()) % bezierPoints.size()]->M;
	}
	calculateHandlePoints();
}

std::vector<CubicBezier*> BezierLoop::generateBezierLoop(int n) {
	std::vector<CubicBezier*> genBeziers;
	for (int i = 0; i < n; i++) {
		//anchor points
		glm::vec3 p0 = glm::vec3(cos(2 * glm::pi<float>() * i / n), sin(2 * glm::pi<float>() * i / n), 0);
		glm::vec3 p3 = glm::vec3(cos(2 * glm::pi<float>() * (i + 1) / n), sin(2 * glm::pi<float>() * (i + 1) / n), 0);

		//control points
		//interesting glitch that looks cool
		//glm::vec3 p1 = glm::vec3(cos(2 * 2*glm::pi<float>()*(i)/n), sin(2 * 2 * glm::pi<float>() * (i)/n), 0);
		//glm::vec3 p2 = glm::vec3(cos(2 * 2*glm::pi<float>()*(i+1)/n), sin(2 * 2 * glm::pi<float>() * (i + 1)/n), 0);
		
		//following makes sure of C1 continuity
		glm::vec3 p1, p2;
		if (i == 0) {
			p1 = 1.1f * p0;
			p2 = 1.1f * p3;
		}
		else if (i < n - 1) {
			p1 = 2.0f * p0 - bezierPoints[3*i- 1];
			p2 = 1.1f * p3;
		}
		else {
			p1 = 2.0f * p0 - bezierPoints[3*i - 1];
			p2 = 2.0f * p3 - bezierPoints[1];
		}
		CubicBezier* bezier = new CubicBezier(p0, p1, p2, p3);
		
		//push back to the points vector
		bezierPoints.push_back(p0);
		bezierPoints.push_back(p1);
		bezierPoints.push_back(p2);

		//finish by pushing back to the beziers vector
		genBeziers.push_back(bezier);
	}
	return genBeziers;
}

//parametric - t between 0 and 1
glm::vec3 BezierLoop::getPointOnLoop(float t) {
	if (t < 0) {
		t = t - (int)t + 1;
	}
	else {
		t = t - (int)t;
	}
	
	int bezierCount = beziers.size();
	int pointBezierIndex = t * bezierCount;
	return beziers[pointBezierIndex]->getPointOnCurve(t * bezierCount - pointBezierIndex);
}

void BezierLoop::calculateHandlePoints() {
	handleLinePoints.clear();
	for (int i = 0; i < beziers.size(); i++) {
		std::vector<glm::vec3> curBezierPoints = beziers[i]->getPoints();
		std::vector<glm::vec3> nextBezierPoints = beziers[(i + 1) % beziers.size()]->getPoints();
		handleLinePoints.push_back(curBezierPoints[2]);
		handleLinePoints.push_back(nextBezierPoints[1]);
	}

	//send new data to gpu
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * handleLinePoints.size(),
		handleLinePoints.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}