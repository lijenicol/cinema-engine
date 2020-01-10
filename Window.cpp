#include "Window.h"
#include "LightSource.h"
#include "Material.h"
#include "Geometry.h"
#include "Node.h"
#include "Transform.h"
#include "CubeMap.h"
#include "CubicBezier.h"
#include "BezierLoop.h"
#include "FrameBuffer.h"
#include "RenderQuad.h"
#include "Camera.h"

int Window::width;
int Window::height;

const char* Window::windowTitle = "Cinema Engine || Elijah Nicol";
double previousTime;

//updated SCENE GRAPH specs
Transform* worldTransform;

//CubeMap
CubeMap* skyBox;

//interactive variables
bool isMouseDown;
glm::vec3 w;	//for trackball rotation
int sceneState;	//for which objects to rotate when
bool isMovementPaused;
bool isFirstPerson;

glm::mat4 Window::projection; // Projection matrix.

//CAMERA ATTRIBUTES
glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.
glm::vec3 originalEye(0, 0, 20); // Camera position.
glm::vec3 originalUp(0, 1, 0); // The up direction of the camera.
glm::vec3 movementKeyVelocity(0, 0, 0);

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

//projection view variables
double originalFov = 60.0;
double fov = 60.0;
double nearZ = 1.0;
double farZ = 100.0;
double Window::ratio;
glm::vec3 dir;
bool viewFrustrumCullingMode;
bool frustrumDemoMode;
int robotCountInView, partCountInView;

std::vector<glm::vec3> planePoints;
std::vector<glm::vec3> planeNormals;

GLuint Window::program; // The shader program id.
GLuint Window::cubeMapProgram; // The cubemap program id.
GLuint Window::bezierShaderProgram; // The Bezier Shader Program id
GLuint frameBufferProgram;
GLuint Window::deferredLightingProgram;
GLuint gaussianBlurProgram;

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::modelLoc; // Location of model in shader.
GLuint Window::camPosLoc;
GLuint Window::skyProjectionLoc; 
GLuint Window::skyViewLoc;
GLuint Window::bezierProjectionLoc;
GLuint Window::bezierViewLoc;
GLuint Window::bezierModelLoc;
GLuint Window::bezierColorLoc;
GLuint Window::deferredCamPosLoc;
GLuint displayFogLoc;
GLuint displayBloomLoc;
GLuint displayBlurLoc;
GLuint Window::prevViewLoc;
GLuint Window::prevModelLoc;
GLuint deltaTimeLoc;

GLuint normalRenderLoc;	//normal render bool location
bool normalRender;	//normal render bool

//FRAME BUFFER
FrameBuffer *frameBuffer;
RenderQuad* renderQuad;
unsigned int gBuffer;
unsigned int gPosition;
unsigned int gNormal;
unsigned int gAlbedoSpec;
unsigned int gGlow;
unsigned int gVelocity;
unsigned int selectedGBuffer;
unsigned int bloomFBO;
unsigned int colorTexture;
unsigned int highlightsTexture;
unsigned int pingpongFBO[2];
unsigned int pingpongBuffer[2];
unsigned int skyBuffer;

//debug bools
bool displayFog;
bool displayBloom;
bool displayMotionBlur;

////SCENE
std::vector<Scene*> Window::sceneList;
Scene* Window::activeScene;

float deltaTime;
float frameDeltaTime;
float prevFrameTime;

bool Window::initializeProgram() {
	// COMPILE ALL SHADERS
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	cubeMapProgram = LoadShaders("shaders/shaderSky.vert", "shaders/shaderSky.frag");
	bezierShaderProgram = LoadShaders("shaders/shaderBezier.vert", "shaders/shaderBezier.frag");
	frameBufferProgram = LoadShaders("shaders/framebufferShader.vert", "shaders/framebufferShader.frag");
	deferredLightingProgram = LoadShaders("shaders/deferredLightingShader.vert", "shaders/deferredLightingShader.frag");
	gaussianBlurProgram = LoadShaders("shaders/gaussianBlurShader.vert", "shaders/gaussianBlurShader.frag");

	// CHECK IF ALL SHADERS COMPILED CORRECT
	if (!program || !cubeMapProgram || !bezierShaderProgram || !frameBufferProgram || !deferredLightingProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// FIND LOCATIONS OF ALL UNIFORMS
	glUseProgram(program);
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	prevViewLoc = glGetUniformLocation(program, "prevView");
	modelLoc = glGetUniformLocation(program, "model");
	prevModelLoc = glGetUniformLocation(program, "prevModel");
	deltaTimeLoc = glGetUniformLocation(program, "deltaTime");
	glUniform1i(glGetUniformLocation(program, "backgroundTexture"), 0);
	glUniform1i(glGetUniformLocation(program, "texR"), 1);
	glUniform1i(glGetUniformLocation(program, "texG"), 2);
	glUniform1i(glGetUniformLocation(program, "texB"), 3);
	glUniform1i(glGetUniformLocation(program, "blendMap"), 4);

	glUseProgram(cubeMapProgram);
	skyProjectionLoc = glGetUniformLocation(cubeMapProgram, "projection");
	skyViewLoc = glGetUniformLocation(cubeMapProgram, "view");

	glUseProgram(bezierShaderProgram);
	bezierProjectionLoc = glGetUniformLocation(bezierShaderProgram, "projection");
	bezierViewLoc = glGetUniformLocation(bezierShaderProgram, "view");
	bezierModelLoc = glGetUniformLocation(bezierShaderProgram, "model");
	bezierColorLoc = glGetUniformLocation(bezierShaderProgram, "modelColor");

	glUseProgram(deferredLightingProgram);
	glUniform1i(glGetUniformLocation(deferredLightingProgram, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(deferredLightingProgram, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(deferredLightingProgram, "gAlbedoSpec"), 2);
	glUniform1i(glGetUniformLocation(deferredLightingProgram, "gGlow"), 3);
	glUniform1i(glGetUniformLocation(deferredLightingProgram, "gSky"), 4);
	displayFogLoc = glGetUniformLocation(deferredLightingProgram, "fog");
	deferredCamPosLoc = glGetUniformLocation(deferredLightingProgram, "viewPos");

	glUseProgram(frameBufferProgram);
	glUniform1i(glGetUniformLocation(frameBufferProgram, "screenTexture"), 0);
	glUniform1i(glGetUniformLocation(frameBufferProgram, "bloomBlur"), 1);
	glUniform1i(glGetUniformLocation(frameBufferProgram, "velocityTexture"), 2);
	displayBloomLoc = glGetUniformLocation(frameBufferProgram, "bloom");
	displayBlurLoc = glGetUniformLocation(frameBufferProgram, "blur");
	
	//Set previous time
	previousTime = glfwGetTime();
	prevFrameTime = glfwGetTime();

	//init vars that need to
	isMouseDown = false;
	w.x = 0;
	w.y = 0;
	w.z = 0;
	sceneState = 1;
	dir = glm::normalize(center - eye);
	viewFrustrumCullingMode = false;
	frustrumDemoMode = false;
	partCountInView = 0;
	robotCountInView = 0;
	isMovementPaused = false;
	isFirstPerson = false;
	displayFog = true;
	displayBloom = true;
	displayMotionBlur = true;
	deltaTime = 0.1f;

	return true;
}

bool Window::initializeObjects()
{
	// Load in the skybox
	std::vector<std::string> faces
	{
		"textures/skyboxes/palace/right.jpg",
		"textures/skyboxes/palace/left.jpg",
		"textures/skyboxes/palace/top.jpg",
		"textures/skyboxes/palace/bottom.jpg",
		"textures/skyboxes/palace/front.jpg",
		"textures/skyboxes/palace/back.jpg",
	};

	skyBox = new CubeMap(faces);

	Scene* scene = new Scene();
	Window::activeScene = scene;
	Window::sceneList.push_back(scene);
	
	//generate framebuffers
	frameBuffer = new FrameBuffer(width, height, 1);
	renderQuad = new RenderQuad();

	//generate buffers
	setupGBuffer();
	setupBloomBuffer();
	setupPingpongBuffer();
	selectedGBuffer = 0;

	return true;
}

void Window::cleanUp()
{
	// Delete the shader program.
	glDeleteProgram(program);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	int FULL_SCREEN = 0;
	GLFWwindow* window;
	if (FULL_SCREEN) {
		window = glfwCreateWindow(width, height, windowTitle, glfwGetPrimaryMonitor(), NULL);		//ENABLES FULL SCREEN MODE 
	}
	else {
		window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	}

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	ratio = double(width) / (double)height;
	Window::projection = glm::perspective(glm::radians(fov), 
		ratio, nearZ, farZ);
	std::cout << ratio;
	//activeScene->activeCamera->updateProjection(fov, ratio, nearZ, farZ);
}

void Window::idleCallback()
{
	deltaTime = glfwGetTime() - previousTime;
	activeScene->activeCamera->updateCameraPosition(movementKeyVelocity, deltaTime);
	activeScene->update(deltaTime);
	previousTime = glfwGetTime();
}

//MAIN RENDER FUNCTION
void Window::displayCallback(GLFWwindow* window)
{	
	// RENDER OBJECTS TO FRAMEBUFFER
	glUseProgram(program);
	frameDeltaTime = glfwGetTime() - prevFrameTime;
	glUniform1f(deltaTimeLoc, frameDeltaTime);
	prevFrameTime = glfwGetTime();
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	activeScene->draw();
	
	//print out errors
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}

	//RENDER THE SKYBOX BEFORE POSTPROCESSING EFFECTS
	//need to copy depth buffer from gBuffer to bloomFBO buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->fbo); // write depth to sky framebuffer
	glBlitFramebuffer(
		0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->fbo);
	glUseProgram(cubeMapProgram);
	activeScene->skyBox->draw();

	//BIND TO ANOTHER FRAMEBUFFER FOR ALL LIGHTING CALCULATIONS
	glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(deferredLightingProgram);
	//bind textures and write to uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gGlow);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, *frameBuffer->tex);
	glUniform3fv(deferredCamPosLoc, 1, glm::value_ptr(activeScene->activeCamera->eye));
	glUniform1i(displayFogLoc, displayFog);
	//draw to framebuffer
	renderQuad->draw();
	glActiveTexture(GL_TEXTURE0);

	//NOW THERE EXISTS TWO DIFFERENT BUFFERS - one for color and one for the highlights
	//we want to blur the highlights buffer
	bool horizontal = true, first_iteration = true;
	int amount = 6;
	glUseProgram(gaussianBlurProgram);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(gaussianBlurProgram, "horizontal"), horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? highlightsTexture : pingpongBuffer[!horizontal]
		);
		renderQuad->draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//RENDER THE FINAL FRAMEBUFFER WITH ALL POSTPROCESSING EFFECTS
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(frameBufferProgram);
	glUniform1f(glGetUniformLocation(frameBufferProgram, "exposure"), activeScene->activeCamera->exposure);
	glUniform1i(displayBloomLoc, displayBloom);
	glUniform1i(displayBlurLoc, displayMotionBlur);
	glActiveTexture(GL_TEXTURE0);
	if (selectedGBuffer == 0) {
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gVelocity);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, selectedGBuffer + gPosition - 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	renderQuad->draw();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (sceneState == 1) {
		//change fov
		//fov -= yoffset;
		//activeScene->activeCamera->updateProjection(activeScene->activeCamera->fov - yoffset, ratio, nearZ, farZ);

		//change exposure
		activeScene->activeCamera->exposure -= yoffset/30;
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	//handles holding down the left mouse button functionality
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		isMouseDown = !isMouseDown;
	}
}

//first person camera rotation
glm::vec2 prevPoint(0,0);
void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (isMouseDown) {
		activeScene->activeCamera->updateAngle(xpos - prevPoint.x, ypos - prevPoint.y);
	}
	prevPoint = glm::vec2(xpos, ypos);
}

glm::mat4 Window::rotateAboutAxis(float deg, glm::vec3 axis, glm::mat4 model) {
	glm::mat4 rotation = glm::rotate(glm::mat4(1), deg, axis);
	return rotation * model;
}

void Window::calculateFrustrumPlanes() {
	//handle demo mode fov
	double ifov;
	if (frustrumDemoMode)
		ifov = originalFov;
	else
		ifov = fov;

	//calculate near and far plane width and heights
	double Hnear = 2 * tan(glm::radians(ifov) / 2) * nearZ;
	double Wnear = Hnear * ratio;

	double Hfar = 2 * tan(glm::radians(ifov) / 2) * farZ;
	double Wfar = Hfar * ratio;

	//calculating near plane and far plane points
	glm::vec3 nc = eye + dir * (float)nearZ;
	glm::vec3 fc = eye + dir * (float)farZ;

	//calculating normals for all the faces
	glm::vec3 right = glm::cross(dir, up);
	glm::vec3 a = glm::normalize((nc + right * (float)Wnear/2.0f) - eye);
	glm::vec3 normalRight = glm::cross(a, up);
	glm::vec3 b = glm::normalize((nc - right * (float)Wnear / 2.0f) - eye);
	glm::vec3 normalLeft = glm::cross(up, b);
	glm::vec3 c = glm::normalize((nc + up * (float)Hnear / 2.0f) - eye);
	glm::vec3 normalUp = glm::cross(right, c);
	glm::vec3 d = glm::normalize((nc - up * (float)Hnear / 2.0f) - eye);
	glm::vec3 normalDown = glm::cross(d, right);
	glm::vec3 normalFront = -dir;
	glm::vec3 normalBack = dir;

	//push normals to the normal planes list
	planeNormals.clear();
	planeNormals.push_back(normalUp);
	planeNormals.push_back(normalDown);
	planeNormals.push_back(normalLeft);
	planeNormals.push_back(normalRight);
	planeNormals.push_back(normalFront);
	planeNormals.push_back(normalBack);

	//push the points same order as normals
	//most normals intersect the point where the camera is 
	//(hence why eye is pushed multiple times)
	planePoints.clear();
	planePoints.push_back(eye);
	planePoints.push_back(eye);
	planePoints.push_back(eye);
	planePoints.push_back(eye);
	planePoints.push_back(nc);
	planePoints.push_back(fc);
}

bool Window::calculateInsideFrustrum(float r, glm::vec3 pos) {
	if (!viewFrustrumCullingMode)
		return true;

	for (int i = 0; i < 6; i++) {
		float distanceToPlane = glm::dot((pos - planePoints[i]), planeNormals[i]);
		//if the sphere is completely outside of the frustrum then return false
		//std::cout << distanceToPlane << std::endl;
		if (distanceToPlane > r) {
			return false;
		}
	}

	partCountInView++;
	return true;
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float sprintModifier = 6.0f;
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_G:
			selectedGBuffer += 1;
			selectedGBuffer = selectedGBuffer % 6;
			break;
		case GLFW_KEY_F:
			displayFog = !displayFog;
			break;
		case GLFW_KEY_B:
			displayBloom = !displayBloom;
			break;
		case GLFW_KEY_M:
			displayMotionBlur = !displayMotionBlur;
			break;
		case GLFW_KEY_W:
			movementKeyVelocity += glm::vec3(0, 0, 1);
			break;
		case GLFW_KEY_A:
			movementKeyVelocity += glm::vec3(-1, 0, 0);
			break;
		case GLFW_KEY_S:
			movementKeyVelocity += glm::vec3(0, 0, -1);
			break;
		case GLFW_KEY_D:
			movementKeyVelocity += glm::vec3(1, 0, 0);
			break;
		case GLFW_KEY_SPACE:
			movementKeyVelocity += glm::vec3(0, 1, 0);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			movementKeyVelocity += glm::vec3(0, -1, 0);
			break;
		case GLFW_KEY_LEFT_SHIFT:
			activeScene->activeCamera->movementVelocity *= sprintModifier;
			break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_W:
			movementKeyVelocity -= glm::vec3(0, 0, 1);
			break;
		case GLFW_KEY_A:
			movementKeyVelocity -= glm::vec3(-1, 0, 0);
			break;
		case GLFW_KEY_S:
			movementKeyVelocity -= glm::vec3(0, 0, -1);
			break;
		case GLFW_KEY_D:
			movementKeyVelocity -= glm::vec3(1, 0, 0);
			break;
		case GLFW_KEY_SPACE:
			movementKeyVelocity -= glm::vec3(0, 1, 0);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			movementKeyVelocity -= glm::vec3(0, -1, 0);
			break;
		case GLFW_KEY_LEFT_SHIFT:
			activeScene->activeCamera->movementVelocity /= sprintModifier;
			break;
		default:
			break;
		}
	}
}

void Window::setupGBuffer() {
	//CREATE G BUFFER FOR DEFERRED SHADING
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - glow color buffer
	glGenTextures(1, &gGlow);
	glBindTexture(GL_TEXTURE_2D, gGlow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gGlow, 0);

	// - velocity color buffer
	glGenTextures(1, &gVelocity);
	glBindTexture(GL_TEXTURE_2D, gVelocity);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gVelocity, 0);
	
	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	//generate depth buffer for depth testing
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::setupBloomBuffer() {
	//CREATE BLOOM FBO FOR SPLIT RENDERING
	glGenFramebuffers(1, &bloomFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);

	// color buffer to write to
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	// - highlight buffer to write to 
	glGenTextures(1, &highlightsTexture);
	glBindTexture(GL_TEXTURE_2D, highlightsTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, highlightsTexture, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::setupPingpongBuffer() {
	//generate buffers and textures
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);

	//setup textures
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}