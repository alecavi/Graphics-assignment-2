#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../attribute_array.h"
#include "../texture/image.h"
#include "../texture/texture.h"
#include "data.h"

class Object {
	VertexArray vertices;
	NormalArray normals;
	TexCoordArray texCoords;
	texture::Texture texture;
	GLuint vertexCount;

	Object(
		VertexArray vertices, NormalArray normals, TexCoordArray texCoords, 
		texture::Texture texture, GLuint vertexCount
	) : 
		vertices(std::move(vertices)), normals(std::move(normals)), texCoords(std::move(texCoords)),
		texture(std::move(texture)), vertexCount(vertexCount)
	{}

public:
	explicit Object(ObjectData const& data) {
		*this = Builder(data).build();
	}

	void draw(int drawMode) const {
		this->texture.bind();
		this->vertices.bind();
		this->normals.bind();
		this->texCoords.bind();

		glPointSize(3.f);

		if (drawMode == 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
		else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

		if (drawMode == 2) { glDrawArrays(GL_POINTS, 0, vertexCount); }
		else { glDrawArrays(GL_TRIANGLES, 0, vertexCount); }
	}

	struct Builder {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		Image texture;

		explicit Builder(ObjectData const& data) {
			auto vertexCount = 0;
			for (auto& shape : data.shapes) {
				vertexCount += shape.mesh.num_face_vertices.size() * 3; // 3 vertices for each face
			}

			auto vertices = std::vector<glm::vec3>();
			vertices.reserve(vertexCount);
			auto normals = std::vector<glm::vec3>();
			normals.reserve(vertexCount);
			auto texCoords = std::vector<glm::vec2>();
			texCoords.reserve(vertexCount);

			for (auto& shape : data.shapes) {
				size_t lastProcessedIndex = 0;
				for (auto faceVertexCount : shape.mesh.num_face_vertices) {
					for (size_t v = lastProcessedIndex; v < lastProcessedIndex + faceVertexCount; v++) {
						auto idx = shape.mesh.indices[v]; //get the indices vertex/normal/texCoord

						auto& attrib = data.attrib;

						vertices.push_back(glm::vec3(
							attrib.vertices[3 * idx.vertex_index],
							attrib.vertices[3 * idx.vertex_index + 1],
							attrib.vertices[3 * idx.vertex_index + 2]
						));

						normals.push_back(glm::vec3(
							attrib.normals[3 * idx.normal_index],
							attrib.normals[3 * idx.normal_index + 1],
							attrib.normals[3 * idx.normal_index + 2]
						));

						texCoords.push_back(glm::vec2(
							attrib.texcoords[2 * idx.texcoord_index],
							attrib.texcoords[2 * idx.texcoord_index + 1]
						));
					}
					lastProcessedIndex += faceVertexCount;
				}
				this->vertices = std::move(vertices);
				this->normals = std::move(normals);
				this->texCoords = std::move(texCoords);
				this->texture = Image(data.material.diffuse_texname.c_str());
			}
		}

		Object build() const {
			return Object(
				VertexArray(this->vertices),
				NormalArray(this->normals),
				TexCoordArray(this->texCoords),
				texture::Texture(this->texture),
				this->vertices.size()
			);
		}
	};
};