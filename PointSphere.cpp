#include "PointSphere.h"
#include "Window.h"

PointSphere::PointSphere(glm::vec3 color) : Geometry("sphere.obj", NULL), defaultColor(color) {
	PointSphere::color = defaultColor;
	isSelected = false;
}

//overrides the parent method
void PointSphere::draw(glm::mat4 C) {
	//renders a point sphere with flat shading
	glUseProgram(Window::bezierShaderProgram);
	glm::mat4 i = glm::mat4(1);
	glUniformMatrix4fv(Window::bezierModelLoc, 1, GL_FALSE, glm::value_ptr(i));
	glUniformMatrix4fv(Window::bezierViewLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(Window::bezierProjectionLoc, 1, GL_FALSE, glm::value_ptr(Window::projection));
	glUniform3f(Window::bezierColorLoc, color.x, color.y, color.z);

	//render object
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw the model
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PointSphere::selectionStatus(bool state) {
	isSelected = state;
	if (isSelected) {
		color = glm::vec3(1, 1, 0);
	}
	else {
		color = defaultColor;
	}
}