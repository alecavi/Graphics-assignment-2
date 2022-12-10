#pragma once

#include <array>
#include <cassert>

#include <glad/glad.h>

#include "image.h"

class Cubemap {
	GLuint name;
public:
	Cubemap() : name(0) {}

	Cubemap(std::array<Image, 6> const& faces) {
		GLuint name;
		glGenTextures(1, &name);
		glBindTexture(GL_TEXTURE_CUBE_MAP, name);

		GLuint i = 0;
		for (auto& face : faces) {
			assert(face.getBytes());
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				face.getPixelFormat(), face.getWidth(), face.getHeight(),
				0, face.getPixelFormat(), GL_UNSIGNED_BYTE, face.getBytes()
			);
			i++;
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		this->name = name;
	}

	Cubemap(Cubemap const&) = delete;
	Cubemap& operator=(Cubemap const&) = delete;
	Cubemap(Cubemap&& from) noexcept {
		*this = std::move(from);
	}
	Cubemap& operator=(Cubemap&& from) noexcept {
		this->name = from.name;
		from.name = 0;
		return *this;
	}
	~Cubemap() {
		if (this->name != 0) glDeleteTextures(1, &this->name);
	}

	void bind() const {
		glBindTexture(GL_TEXTURE_2D, this->name);
	}
};