#pragma once

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "../../tiny_obj_loader.h"

struct ObjectData {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	tinyobj::material_t material;

	explicit ObjectData(char const* path) {
		//multiple materials not supported owing to the apparently extreme complexity of doing so
		std::vector<tinyobj::material_t> materials;
		std::string err;
		std::string warn;
		bool ret = tinyobj::LoadObj(&this->attrib, &this->shapes, &materials, &err, &warn, path);
		if (!err.empty()) {
			std::cerr << err << std::endl;
		}
		if (!warn.empty()) {
			std::cerr << err << std::endl;
		}
		if (!ret) {
			std::cerr << "fatal error while loading \"" << path << "\", exiting" << std::endl;
			exit(1);
		}
		if (materials.size() == 1) {
			this->material = std::move(materials[0]);
		} else if (materials.size() > 1) {
			std::cerr << "fatal error while loading \"" << path <<
				"\": At most 1 material is supported. Exiting" << std::endl;
			exit(1);
		}
	}
};