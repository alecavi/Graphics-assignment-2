#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../attribute_array.h"
#include "data.h"

class ObjectPosition {
	VertexArray vertices;
	GLuint vertexCount;

	ObjectPosition(VertexArray vertices, GLuint vertexCount) :
		vertices(std::move(vertices)), vertexCount(vertexCount)
	{}

public:
	explicit ObjectPosition(ObjectData const& data) {
		*this = Builder(data).build();
	}

	void draw(int drawMode) const {
		this->vertices.bind();

		glPointSize(3.f);

		if (drawMode == 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
		else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

		if (drawMode == 2) { glDrawArrays(GL_POINTS, 0, vertexCount); }
		else { glDrawArrays(GL_TRIANGLES, 0, vertexCount); }
	}

	struct Builder {
		std::vector<glm::vec3> vertices;

		explicit Builder(ObjectData const& data) {
			auto vertexCount = 0;
			for (auto& shape : data.shapes) {
				vertexCount += shape.mesh.num_face_vertices.size() * 3; // 3 vertices for each face
			}

			auto vertices = std::vector<glm::vec3>();
			vertices.reserve(vertexCount);

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
					}
					lastProcessedIndex += faceVertexCount;
				}
			}
			this->vertices = std::move(vertices);
		}

		ObjectPosition build() const {
			return ObjectPosition(
				VertexArray(this->vertices),
				this->vertices.size()
			);
		}
	};
};