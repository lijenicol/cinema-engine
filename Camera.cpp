#include "Camera.h"
#include "Window.h"

//initialize camera
Camera::Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fov, float nearZ, float farZ) 
	: eye(eye), center(center), up(up), fov(fov), nearZ(nearZ), farZ(farZ) {
	//init matrices
	view = glm::lookAt(eye, center, up);
	lastView = view;
	lastFrameView = view;
	ratio = Window::ratio;
	projection = glm::perspective(glm::radians(fov), ratio, nearZ, farZ);
	
	//init movement attributes
	roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
	movementVelocity = glm::vec3(0.3,0.3,0.3);
	exposure = 1.0f;
}

Camera::~Camera() {}

//updates the view matrix by calculating it ourselves
void Camera::updateView() {
	//rotation matrices
	glm::mat4 mRoll = glm::rotate(glm::mat4(1), roll, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 mPitch = glm::rotate(glm::mat4(1), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mYaw = glm::rotate(glm::mat4(1), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

	//combine into one rotation
	glm::mat4 mRotation = mRoll * mPitch * mYaw;

	//calculate translation matrix
	glm::mat4 mTranslation = glm::translate(glm::mat4(1), -eye);
	
	//calculate final view matrix
	lastView = view;
	view = mRotation * mTranslation;
}

void Camera::updateProjection(float nFov, float nRatio, float nNearZ, float nFarZ) {
	fov = nFov;
	ratio = nRatio;
	nearZ = nNearZ;
	farZ = nFarZ;
	projection = glm::perspective(glm::radians(nFov), nRatio, nNearZ, nFarZ);
}

//updates camera position based on direction
void Camera::updateCameraPosition(glm::vec3 moveDirection, float deltaTime) {
	//calculate forward and strafe vectors from view matrix
	glm::vec3 forward(view[0][2], view[1][2], view[2][2]);
	glm::vec3 strafe(view[0][0], view[1][0], view[2][0]);
	glm::vec3 upward(view[0][1], view[1][1], view[2][1]);
	eye += (-moveDirection.z * forward + moveDirection.x * strafe + moveDirection.y * upward) * movementVelocity * deltaTime;

	updateView();
}

//updates camera yaw and pitch
void Camera::updateAngle(float dYaw, float dPitch) {
	const float mouseSensitivityX = 0.01f;
	const float mouseSensitivityY = 0.01f;

	yaw += mouseSensitivityX * dYaw;
	pitch += mouseSensitivityY * dPitch;

	updateView();
}