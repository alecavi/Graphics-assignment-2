#pragma once

#include "objects/program.h"
#include "objects/texture/texture.h"

// Store the program used by the objects
struct ObjectProgram {
	Program program;
	UniformLocation model;
	UniformLocation view;
	UniformLocation projection;
	UniformLocation cameraPosition;
	UniformLocation lightPosition;

	ObjectProgram(char const* vertexPath, char const* fragmentPath) {
		auto program = Program(vertexPath, fragmentPath);
		program.getUniformLocation("tex").set(texture::Texture::UNIT);
		this->model = program.getUniformLocation("model");
		this->view = program.getUniformLocation("view");
		this->projection = program.getUniformLocation("projection");
		this->cameraPosition = program.getUniformLocation("cameraPosition_W");
		this->lightPosition = program.getUniformLocation("lightPosition_W");
		this->program = std::move(program);
	}
};

// Store the program used by the skybox
struct SkyboxProgram {
	Program program;
	UniformLocation view;
	UniformLocation projection;

	SkyboxProgram(char const* vertexPath, char const* fragmentPath) {
		auto program = Program(vertexPath, fragmentPath);
		program.getUniformLocation("skybox").set(texture::Texture::UNIT);
		this->view = program.getUniformLocation("view");
		this->projection = program.getUniformLocation("projection");
		this->program = std::move(program);
	}
};

// Stores the program used by the ground plane
struct GroundProgram {
	Program program;
	UniformLocation model;
	UniformLocation view;
	UniformLocation projection;
	UniformLocation colourMode;

	GroundProgram(char const* vertexPath, char const* fragmentPath) {
		auto program = Program(vertexPath, fragmentPath);
		program.getUniformLocation("tex").set(texture::Texture::UNIT);
		program.getUniformLocation("normalMap").set(texture::NormalMap::UNIT);
		this->model = program.getUniformLocation("model");
		this->view = program.getUniformLocation("view");
		this->projection = program.getUniformLocation("projection");
		this->colourMode = program.getUniformLocation("colourMode");
		this->program = std::move(program);
	}
};

// Stores the program used by the light source
struct LightProgram {
	Program program;
	UniformLocation model;
	UniformLocation view;
	UniformLocation projection;

	LightProgram(char const* vertexPath, char const* fragmentPath) {
		auto program = Program(vertexPath, fragmentPath);
		this->model = program.getUniformLocation("model");
		this->view = program.getUniformLocation("view");
		this->projection = program.getUniformLocation("projection");
		this->program = std::move(program);
	}
};