#pragma once

#include <array>
#include <cassert>

#include <glad/glad.h>

#include "image.h"

namespace texture {

	template<typename Kind>
	class Texture2D {
		GLuint name;
	public:
		static const GLuint UNIT = Kind::UNIT;

		Texture2D() : name(0) {}

		Texture2D(Image const& image) {
			assert(image.getBytes());
			GLuint name;
			glGenTextures(1, &name);
			glActiveTexture(UNIT);
			glBindTexture(GL_TEXTURE_2D, name);
			glTexImage2D(
				GL_TEXTURE_2D, 0, image.getPixelFormat(), image.getWidth(),
				image.getHeight(), 0, image.getPixelFormat(), GL_UNSIGNED_BYTE, image.getBytes());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			this->name = name;
		}

		Texture2D(Texture2D const&) = delete;
		Texture2D& operator=(Texture2D const&) = delete;
		Texture2D(Texture2D&& from) noexcept {
			*this = std::move(from);
		}
		Texture2D& operator=(Texture2D&& from) noexcept {
			this->name = from.name;
			from.name = -1;
			return *this;
		}
		~Texture2D() {
			if (this->name != -1) { glDeleteTextures(1, &this->name); }
		}

		void bind() const {
			glActiveTexture(UNIT);
			glBindTexture(GL_TEXTURE_2D, this->name);
		}
	};
	struct TextureKind {
		static const GLuint UNIT = 0;
	};
	struct NormalMapKind {
		static const GLuint UNIT = 1;
	};

	typedef Texture2D<TextureKind> Texture;
	typedef Texture2D<NormalMapKind> NormalMap;
}