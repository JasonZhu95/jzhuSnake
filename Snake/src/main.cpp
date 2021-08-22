#include <stdio.h>
#include <thread>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_window_iconify_callback(GLFWwindow* window, int iconified);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 600;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

static Game Snake;
static bool minimized = false;
static bool fullscreen = false;

int main(int argc, char* argv[])
{
	glfwInit();

	// Using OpenGL at least version 3.3 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	glfwWindowHint(GLFW_RESIZABLE, true);

	// Create a Window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Debug Check if Glad Failed to Initialize
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Check for key inputs
	glfwSetKeyCallback(window, key_callback);

	// Check and resize window size
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetWindowIconifyCallback(window, glfw_window_iconify_callback);

	// No v-sync.
	glfwSwapInterval(0);

	// Initialize Snake Game
	Snake.init(SCREEN_WIDTH, SCREEN_HEIGHT, 15);

	// deltaTime variables
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float maxFrameTime = 1.0f / 60.0;

	while (!glfwWindowShouldClose(window))
	{
		// calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Poll User Events
		glfwPollEvents();

		if (!minimized)
		{
			// User Input
			Snake.processInput();

			// Update Game Events
			Snake.update(deltaTime);

			// Render Game
			Snake.render();
		}

		glfwSwapBuffers(window);
		double time_elapsed_for_frame = glfwGetTime() - currentFrame;

		if (time_elapsed_for_frame < maxFrameTime)
		{
			double ms = (maxFrameTime - time_elapsed_for_frame) * 1000.0;

			if (ms > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(unsigned long(ms)));
		}
	}

	// Delete all loaded memory
	Snake.endGame();

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Close Window on ESC Press
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Snake.Keys[key] = true;
		else if (action == GLFW_RELEASE)
			Snake.Keys[key] = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Snake.updateDimensions(width, height);
}
void glfw_window_iconify_callback(GLFWwindow* window, int iconified)
{
	minimized = iconified;
}