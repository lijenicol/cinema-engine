//the camera class holds all attributes and methods related to the camera
//all view frustrum calculations are handled in this class

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Camera
{
private:
	//camera attributes
	float nearZ, farZ, ratio;
	float roll, pitch, yaw;

	//view frustrum attributes
	std::vector<glm::vec3> planePoints;
	std::vector<glm::vec3> planeNormals;

	//private update methods
	void updateView();
public:
	Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up, float fov, float nearZ, float farZ);
	~Camera();

	glm::vec3 movementVelocity;
	glm::vec3 eye, center, up;
	glm::mat4 view, projection;
	glm::mat4 lastView;
	glm::mat4 lastFrameView;
	float fov, exposure;

	//update methods
	void updateProjection(float nFov, float nRatio, float nNearZ, float nFarZ);
	void updateCameraPosition(glm::vec3 moveDirection, float deltaTime);
	void updateAngle(float dYaw, float dPitch);
};

