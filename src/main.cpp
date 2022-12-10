#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <stack>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "objects/object/object.h"
#include "objects/object/object_position.h"
#include "objects/skybox.h"
#include "objects/texture/texture.h"
#include "programs.h"
#include "window.h"

struct Camera {
	glm::vec3 position;
	glm::vec3 lookDirection;
	glm::vec3 up;
	GLfloat speed;
	GLfloat pitch;
	GLfloat yaw;
	GLfloat turnSensitivity;
	bool firstFrame;

	Camera(
		glm::vec3 position, glm::vec3 lookDirection, glm::vec3 up, GLfloat speed, GLfloat turnSensitivity
	) :
		position(position), lookDirection(lookDirection), up(up), speed(speed), pitch(0.f), yaw(-90.f),
		turnSensitivity(turnSensitivity), firstFrame(true)
	{}
};

// Package together all the data that needs to be shared across several GLFW callbacks
// so it can be made available to them through GLFW's user pointer mechanism
struct RenderData {
	Camera camera;
	glm::vec3 lightPosition;
	glm::vec2 lastMousePos;
	GLfloat aspectRatio;
	GLuint drawMode;
	GLfloat timeDelta;

	RenderData(
		Camera camera, glm::vec3 lightPosition, GLfloat screenWidth, GLfloat screenHeight, GLuint drawMode
	) :
		camera(camera), lightPosition(lightPosition), 
		lastMousePos(glm::vec2(screenWidth / 2.f, screenHeight / 2.f)), 
		aspectRatio(screenWidth / screenHeight), drawMode(drawMode), timeDelta(0)
	{}
};

// display callback, used in the event loop
void display(
	RenderData& data, 
	ObjectProgram& objectProgram, Object const& cube, Object const& rubik,
	SkyboxProgram& skyboxProgram, Skybox const& skybox,
	LightProgram& lightProgram, ObjectPosition const& light
	//GroundProgram& groundProgram, NormalMap<Object>& ground
) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	glm::mat4 view = glm::lookAt(
		data.camera.position,
		data.camera.position + data.camera.lookDirection,
		data.camera.up
	);
	glm::mat4 projection = glm::perspective(glm::radians(30.0f), data.aspectRatio, 0.1f, 100.0f);

	// objects
	{
		objectProgram.program.use();

		objectProgram.view.set(view);
		objectProgram.projection.set(projection);
		objectProgram.cameraPosition.set(data.camera.position);
		objectProgram.lightPosition.set(data.lightPosition);

		auto scale = 0.3f;

		auto model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(-1.f, 0.301f, 0.f));
		model = glm::scale(model, glm::vec3(scale));
		objectProgram.model.set(model);
		cube.draw(data.drawMode);

		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(1.f, 0.301f, 0.f));
		model = glm::scale(model, glm::vec3(scale));
		objectProgram.model.set(model);
		rubik.draw(data.drawMode);
	}
	// light
	{
		lightProgram.program.use();
		lightProgram.view.set(view);
		lightProgram.projection.set(projection);

		auto model = glm::mat4(1.f);
		model = glm::translate(model, data.lightPosition);
		model = glm::scale(model, glm::vec3(0.1));
		lightProgram.model.set(model);
		light.draw(data.drawMode);
	}
	// ground
	/*
	{
		groundProgram.program.use();
		groundProgram.model.set(glm::mat4(1.f));
		groundProgram.view.set(view);
		groundProgram.projection.set(projection);
		groundProgram.colourMode.set(1); //TODO: probably remove
		ground.draw(data.drawMode);

	}
	*/
	// skybox
	{
		skyboxProgram.program.use();
		skyboxProgram.view.set(glm::mat4(glm::mat3(view)));
		skyboxProgram.projection.set(projection);
		skybox.draw(data.drawMode);
	}

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

// GLFW's docs recommend using glfwGetKey for things like driving animation,
// and using the key callback for things like text editing instead. 
// (https://www.glfw.org/docs/latest/input_guide.html#input_key)
// I'm not doing text editing here, so key polling it is.
//
// To complicate matters, some buttons are meant to be held (movement keys) and should work
// every frame, while other buttons are meant to be tapped and should thus have a delay to avoid accidentally
// pressing them seven times owing to a high framerate. To account for this, those buttons are handled in a
// key callback, as implementing this functionality in this function would amount to reimplementing existing
// GLFW code
void keyboardPoll(Window<RenderData>& window) {
	auto& data = window.getData();
	auto& camera = data.camera;

	auto speed = camera.speed * data.timeDelta;
	if (window.isKeyPressed(GLFW_KEY_LEFT_CONTROL)) { speed *= 3.f; }

	auto forwards = camera.lookDirection * speed;
	auto sideways = glm::normalize(glm::cross(camera.lookDirection, camera.up)) * speed;
	auto upwards = camera.up * speed;

	auto delta = [&](
		std::array<int, 6> const& keys, glm::vec3 sideways, glm::vec3 upwards, glm::vec3 forwards) -> glm::vec3 
	{
		auto delta = glm::vec3(0.f);
		if (window.isKeyPressed(keys[0])) { delta -= sideways; }
		if (window.isKeyPressed(keys[1])) { delta += sideways; }
		if (window.isKeyPressed(keys[2])) { delta -= upwards; }
		if (window.isKeyPressed(keys[3])) { delta += upwards; }
		if (window.isKeyPressed(keys[4])) { delta += forwards; }
		if (window.isKeyPressed(keys[5])) { delta -= forwards; }
		return delta;
	};

	camera.position += delta(
		{ 'A', 'D', GLFW_KEY_LEFT_SHIFT, ' ', 'W', 'S' }, sideways, upwards, forwards
	);
		
	data.lightPosition += delta(
		{ GLFW_KEY_KP_4, GLFW_KEY_KP_6, GLFW_KEY_KP_8, GLFW_KEY_KP_2, GLFW_KEY_KP_1, GLFW_KEY_KP_3 },
		glm::vec3(speed, 0.f, 0.f),
		glm::vec3(0.f, speed, 0.f),
		glm::vec3(0.f, 0.f, speed)
	);

	if (window.isKeyPressed(GLFW_KEY_ESCAPE)) window.setShouldClose();
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto& data = *(RenderData*)glfwGetWindowUserPointer(window);
	if (key == ',' && action == GLFW_PRESS) {
		data.drawMode++;
		if (data.drawMode > 2) data.drawMode = 0;
	}
}

// Handle mouse movement
void mouseCallback(GLFWwindow* window, double x, double y) {
	auto& data = *((RenderData*)glfwGetWindowUserPointer(window));
	auto& camera = data.camera;

	if (camera.firstFrame) {
		data.lastMousePos = glm::vec2(x, y);
		camera.firstFrame = false;
	}

	float xDelta = x - data.lastMousePos.x;
	float yDelta = data.lastMousePos.y - y;
	data.lastMousePos = glm::vec2(x, y);

	xDelta *= camera.turnSensitivity;
	yDelta *= camera.turnSensitivity;

	camera.yaw += xDelta;
	camera.pitch += yDelta;

	if (camera.pitch > 89.0) { camera.pitch = 89.0; }
	if (camera.pitch < -89.0) { camera.pitch = -89.0; }

	auto direction = glm::vec3(
		cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)),
		sin(glm::radians(camera.pitch)),
		sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch))
	);
	camera.lookDirection = glm::normalize(direction);
}

// handle window resizing
static void reshapeCallback(GLFWwindow* window, int w, int h) {
	auto data = (RenderData*)glfwGetWindowUserPointer(window);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	data->aspectRatio = ((float)w / 640.f*4.f) / ((float)h / 480.f*3.f);
}

int main(int argc, char* argv[]) {
	auto width = 1024;
	auto height = 768;

	auto renderData = RenderData(
		Camera(								// camera
			glm::vec3(0.f, 0.f, 4.f),			// position
			glm::vec3(0.f, 0.f, -1.f),			// look direction 
			glm::vec3(0.f, 1.f, 0.f),			// up direction
			3.f,								// speed
			0.1f								// turn sensitivity
		),
		glm::vec3(0.f),						// light position
		(GLfloat)width,						// screen width
		(GLfloat)height,					// screen height
		0									// draw mode
	);
	
	auto window = Window<RenderData>(width, height, "Sceney mcSceneFace", renderData);
	window.setCursorPosCallback(mouseCallback);
	window.setKeyCallback(keyboardCallback);
	window.setReshapeCallback(reshapeCallback);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	auto objectProgram = ObjectProgram("shaders/object.vert", "shaders/object.frag");
	auto skyboxProgram = SkyboxProgram("shaders/skybox.vert", "shaders/skybox.frag");
	auto lightProgram = LightProgram("shaders/light.vert", "shaders/light.frag");
	//auto groundProgram = GroundProgram("shaders/ground.vert", "shaders/ground.frag");

	auto cube = Object(ObjectData("objects/aof5_cube.obj"));
	auto rubik = Object(ObjectData("objects/rubik.obj"));

	auto light = ObjectPosition(ObjectData("objects/light_sphere.obj"));

	//auto ground = NormalMap<Object>(ObjectData("objects/ground.obj"));

	auto skybox = Skybox({
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg",
	});

	window.eventLoop([&](auto& window) {
		keyboardPoll(window);
		display(window.getData(), 
			objectProgram, cube, rubik,
			skyboxProgram, skybox,
			lightProgram, light
			//groundProgram, ground
		);
	});

	glDeleteVertexArrays(1, &vao);

	return 0;
}




