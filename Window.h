#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "Object.h"
#include "Cube.h"
#include "PointCloud.h"
#include "shader.h"
#include "LightSource.h"
#include "glm/gtx/string_cast.hpp"
#include "CubicBezier.h"
#include "Scene.h"

class Window
{
public:
	static int width;
	static int height;
	static double ratio;
	static const char* windowTitle;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	
	//Shader attributes - Shader IDs and Uniform locations
	static GLuint program, cubeMapProgram, bezierShaderProgram, deferredLightingProgram, projectionLoc, viewLoc, modelLoc, skyViewLoc, skyProjectionLoc, camPosLoc,
		bezierProjectionLoc, bezierViewLoc, bezierModelLoc, bezierColorLoc, deferredCamPosLoc, prevViewLoc, prevModelLoc;

	//Scene attributes
	static std::vector<Scene*> sceneList;
	static Scene* activeScene;

	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void idleCallback();
	static void displayCallback(GLFWwindow*);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static glm::mat4 rotateAboutAxis(float deg, glm::vec3 axis, glm::mat4 model);
	static void calculateFrustrumPlanes();
	static bool calculateInsideFrustrum(float r, glm::vec3 pos);
	
	//Initializes Framebuffers for the GBuffer used in deferred rendering
	static void setupGBuffer();

	//Initializes Framebuffers necessary to create the bloom effect
	static void setupBloomBuffer();

	//Initializes two buffers that are used to perform the Gaussian Blur effect
	static void setupPingpongBuffer();
};

#endif
