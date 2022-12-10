#pragma once

#include <iostream>

#include<glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Image {
	int width;
	int height;
	int channelCount;
	GLenum pixelFormat;
	unsigned char* bytes;
public:
	Image() : width(-1), height(-1), channelCount(-1), pixelFormat(-1), bytes(nullptr) {}

	explicit Image(char const* path) {
		int width;
		int height;
		int channelCount;
		// This is intentionally not freed on the paths that exit from the program
		// The OS will reclaim the memory anyway, and it keeps the code simpler and less prone to bugs
		auto textureBytes = stbi_load(path, &width, &height, &channelCount, 0);
		if (textureBytes == NULL) {
			std::cerr << "Error while loading image \"" << path << ": " << stbi_failure_reason() << std::endl;
			exit(1);
		}
		int pixelFormat;
		switch (channelCount) {
		case 3:
			pixelFormat = GL_RGB;
			break;
		case 4:
			pixelFormat = GL_RGBA;
			break;
		default:
			std::cerr << "Error while processing image \"" << path <<
				"\": Pixel format not recognised. Exiting" << std::endl;
			exit(1);
		}

		this->width = width;
		this->height = height;
		this->channelCount = channelCount;
		this->pixelFormat = pixelFormat;
		this->bytes = textureBytes;
	}

	Image(Image const&) = delete;
	Image& operator=(Image const&) = delete;
	Image(Image&& from) noexcept {
		*this = std::move(from);
	}
	Image& operator=(Image&& from) noexcept {
		this->width = from.width;
		this->height = from.height;
		this->channelCount = from.channelCount;
		this->pixelFormat = from.pixelFormat;
		this->bytes = from.bytes;
		from.bytes = nullptr;
		return *this;
	}
	~Image() {
		if (this->bytes) stbi_image_free(this->bytes);
	}

	int getWidth() const {
		return this->width;
	}
	int getHeight() const {
		return this->height;
	}
	int getChannelCount() const {
		return this->channelCount;
	}
	GLenum getPixelFormat() const {
		return this->pixelFormat;
	}
	unsigned char const* getBytes() const {
		return this->bytes;
	}
};