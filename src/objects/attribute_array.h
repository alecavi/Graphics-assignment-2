#pragma once

#include <vector>

#include <glad/glad.h>

template<typename Attribute>
class AttributeArray {
	GLuint name;

	using Element = typename Attribute::Element;

public:
	AttributeArray() : name(0) {}

	explicit AttributeArray(std::vector<Element> const& data) :
		AttributeArray(data.data(), data.size()) {}

	template<size_t LEN>
	AttributeArray(std::array<Element, LEN> const& data) :
		AttributeArray(data.data(), data.size()) {}

	AttributeArray(Element const* data, GLuint size) {
		glGenBuffers(1, &this->name);
		glBindBuffer(GL_ARRAY_BUFFER, this->name);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(Element), &data[0], GL_STATIC_DRAW);
	}

	AttributeArray(AttributeArray const&) = delete;
	AttributeArray& operator=(AttributeArray const&) = delete;
	AttributeArray(AttributeArray&& from) noexcept {
		*this = std::move(from);
	}
	AttributeArray& operator=(AttributeArray&& from) noexcept {
		this->name = from.name;
		from.name = -1;
		return *this;
	}
	~AttributeArray() {
		if (this->name != -1) { glDeleteBuffers(1, &this->name); }
	}

	void bind() const {
		glEnableVertexAttribArray(Attribute::ATTRIBUTE);
		glBindBuffer(GL_ARRAY_BUFFER, this->name);
		glVertexAttribPointer(Attribute::ATTRIBUTE, Attribute::SIZE, GL_FLOAT, GL_FALSE, 0, 0);
	}
};

struct AttributePosition {
	typedef glm::vec3 Element;
	static const GLuint ATTRIBUTE = 0;
	static const GLuint SIZE = 3;
};
struct AttributeNormal {
	typedef glm::vec3 Element;
	static const GLuint ATTRIBUTE = 1;
	static const GLuint SIZE = 3;
};
struct AttributeTexCoord {
	typedef glm::vec2 Element;
	static const GLuint ATTRIBUTE = 2;
	static const GLuint SIZE = 2;
};
class AttributeTangent {
	typedef glm::vec3 Element;
	static const GLuint ATTRIBUTE = 3;
	static const GLuint SIZE = 3;
};

typedef AttributeArray<AttributePosition> VertexArray;
typedef AttributeArray<AttributeNormal> NormalArray;
typedef AttributeArray<AttributeTexCoord> TexCoordArray;
typedef AttributeArray<AttributeTangent> TangentArray;