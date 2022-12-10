// Alessandro Cavicchioli, 2022-11-19

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Represents a constructed GLFW window and OpenGL context.
template<typename RenderData>
class Window {
private:
	GLFWwindow* window;
public: 
	//Construct a window, and borrow render data to be exposed to the callbacks.
	//Note that this class does not copy the render data, and considers it the caller's
	//responsibility to ensure it lives long enough
	Window(int width, int height, char const* title, RenderData& data) {
		glfwSetErrorCallback([](int error, char const* err_data) {
			std::cout << "GLFW error:" << error << " - " << err_data << std::endl;
			});

		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW." << std::endl;
			exit(EXIT_FAILURE);
		}
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
		glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

		this->window = glfwCreateWindow(width, height, title, 0, 0);
		if (!this->window) {
			std::cerr << "Could not open GLFW window." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(this->window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD." << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetWindowTitle(this->window, "Window mcWindowyFace");
		glfwSetWindowUserPointer(window, &data);

		glfwSetInputMode(this->window, GLFW_STICKY_KEYS, true);

		glEnable(GL_MULTISAMPLE);
	}

	// A window uniquely manages a GLFW context that cannot be copied
	Window(Window const&) = delete;
	Window& operator=(Window const&) = delete;

	//But it can be moved
	Window(Window&& from) noexcept {
		*this = std::move(from);
	}
	Window& operator=(Window&& from) noexcept {
		this->window = from.window;
		from.window = nullptr;
		return *this;
	}

	// Destroy the window, and clean up the glfw context along with it
	~Window() {
		if(this->window) glfwTerminate();
	}

	// Display the OPENGL version to standard output.
	static void DisplayVersion() {
		int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MAJOR_VERSION, &minor);
		std::cout << "OpenGL Version = " << major << "." << minor << std::endl;

		/* A more detailed way to the version strings*/
		std::cout << "Vender: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
		std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
	}

	// Set the callback that should be invoked when the window is resized
	void setReshapeCallback(GLFWframebuffersizefun callback) const {
		glfwSetFramebufferSizeCallback(this->window, callback);
	}

	// Set the callback that should be invoked on user keyboard input
	void setKeyCallback(GLFWkeyfun callback) const {
		glfwSetKeyCallback(this->window, callback);
	}

	// Set the callback that should be invoked on user scroll wheel input
	void setScrollCallback(GLFWscrollfun callback) const {
		glfwSetScrollCallback(this->window, callback);
	}

	void setCursorPosCallback(GLFWcursorposfun callback) const {
		glfwSetCursorPosCallback(this->window, callback);
	}

	// Set the callback that should be invoked in case of a GLFW error.
	// This function is static, as the error callback is global and not window-specific
	static void setErrorCallback(GLFWerrorfun callback) {
		glfwSetErrorCallback(callback);
	}

	// Enter the event loop, using the specified render function.
	// the renderer may be a closure, and will be invoked with this window as an argument
	template<typename Renderer>
	void eventLoop(Renderer renderer) {
		auto& data = *(RenderData*)glfwGetWindowUserPointer(this->window);
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		auto lastFrameStart = glfwGetTime();
		while (!glfwWindowShouldClose(this->window)) {
			auto currentFrameStart = glfwGetTime();
			data.timeDelta = currentFrameStart - lastFrameStart;
			lastFrameStart = currentFrameStart;

			renderer(*this);

			glfwSwapBuffers(this->window);
			glfwPollEvents();
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	};

	// Returns a strongly-typed reference to this window's render data
	RenderData& getData() {
		return *(RenderData*)glfwGetWindowUserPointer(this->window);
	}

	// Returns whether the specified key is currently pressed for this window
	bool isKeyPressed(int key) const {
		return glfwGetKey(this->window, key) == GLFW_PRESS;
	}

	// Marks this window as needing to close. It will do so when the next iteration of the event loop starts
	void setShouldClose() {
		glfwSetWindowShouldClose(this->window, true);
	}
};