#include "main.h"
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"

void error_callback(int error, const char* description)
{
	// Print error.
	std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window)
{
	// Set the error callback.
	glfwSetErrorCallback(error_callback);
	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);
	// set the scroll callback
	glfwSetScrollCallback(window, Window::scrollCallback);
	//set mouse callback
	glfwSetMouseButtonCallback(window, Window::mouseButtonCallback);
	//set cursor callback
	glfwSetCursorPosCallback(window, Window::cursorPosCallback);
	//set up key callback
	glfwSetKeyCallback(window, Window::keyCallback);
}

void setup_opengl_settings()
{
	// Enable depth buffering.
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	// Set clear color to black.
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions()
{
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) 
		<< std::endl;

	//If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::cout << "Supported GLSL version is: " << 
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(void)
{
	// Create the GLFW window.
	GLFWwindow* window;
	int FULL_SCREEN = 0;
	if (FULL_SCREEN) {
		window = Window::createWindow(1920, 1080);
	}
	else {
		window = Window::createWindow(640, 480);
	}
	if (!window) exit(EXIT_FAILURE);

	// Print OpenGL and GLSL versions.
	print_versions();
	// Setup callbacks.
	setup_callbacks(window);
	// Setup OpenGL settings.
	setup_opengl_settings();
	// Initialize the shader program; exit if initialization fails.
	if (!Window::initializeProgram()) exit(EXIT_FAILURE);
	// Initialize objects/pointers for rendering; exit if initialization fails.
	if (!Window::initializeObjects()) exit(EXIT_FAILURE);
	
	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
		Window::displayCallback(window);

		// Idle callback. Updating objects, etc. can be done here.
		Window::idleCallback();
	}

	Window::cleanUp();
	// Destroy the window.
	glfwDestroyWindow(window);
	// Terminate GLFW.
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
