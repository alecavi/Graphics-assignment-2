#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class UniformLocation {
	friend class Program;
	GLuint location;
	UniformLocation(GLuint location) : location(location) {}
public:
	UniformLocation() : location(0) {}

	void set(GLuint _1ui) {
		glUniform1ui(this->location, _1ui);
	}
	void set(glm::vec3 _3fv) {
		glUniform3fv(this->location, 1, &_3fv[0]);
	}
	void set(glm::mat4 matrix4fv) {
		glUniformMatrix4fv(this->location, 1, GL_FALSE, &matrix4fv[0][0]);
	}
};

class Program {
public:
	GLuint program;

	// Default constructor: creates an invalid program
	Program() : program(-1) {}

	// Loads and links a program composed of vertex and fragment shaders at the specified paths.
	Program(char const* vertexPath, char const* fragmentPath) {
		//Read shaders
		std::string vertShaderStr = readFile(vertexPath);
		std::string fragShaderStr = readFile(fragmentPath);

		GLuint vertShader;
		GLuint fragShader;
		try {
			vertShader = buildShader(GL_VERTEX_SHADER, vertShaderStr);
			fragShader = buildShader(GL_FRAGMENT_SHADER, fragShaderStr);
		} catch (std::exception& e) {
			std::cout << "Caught exception: " << e.what() << std::endl;
			std::cin.ignore();
			exit(1);
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);


		GLint success = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			GLint logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			auto log = std::string();
			log.reserve(logLength);
			glGetProgramInfoLog(program, logLength, NULL, &log[0]);
			std::cerr << "Error linking program (" << vertexPath << ", "
				<< fragmentPath << "): " << log << std::endl;
		}

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		this->program = program;
	}

	Program(Program const&) = delete;
	Program& operator=(Program const&) = delete;
	Program(Program&& from) noexcept {
		*this = std::move(from);
	}
	Program& operator=(Program&& from) noexcept {
		this->program = from.program;
		from.program = -1;
		return *this;
	}
	~Program() {
		if (this->program != -1) glDeleteProgram(this->program);
	}

	void use() const {
		assert(this->program != -1);
		glUseProgram(this->program);
	}

	UniformLocation getUniformLocation(char const* name) const {
		assert(this->program != -1);
		return UniformLocation(glGetUniformLocation(this->program, name));
	}

private:
	// Build a single shader as the specified shader type, and return its ID.
	static GLuint buildShader(GLenum shaderType, const std::string& shaderText) {
		GLuint shader = glCreateShader(shaderType);
		char const* strFileData = shaderText.c_str();
		glShaderSource(shader, 1, &strFileData, NULL);

		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

			const char* strShaderType = NULL;
			switch (shaderType) {
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			}

			std::cerr << "Compile error in " << strShaderType << "\n\t" << strInfoLog << std::endl;
			delete[] strInfoLog;

			throw std::exception("Shader compile exception");
		}
		return shader;
	}

	// Read the file at the given path, returning its contents as a string
	static std::string readFile(char const* filePath) {
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open()) {
			std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;
	}
};