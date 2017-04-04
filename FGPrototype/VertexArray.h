#pragma once

#include <glad/glad.h>

namespace gfx
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();
		void SetBufferData(
			GLuint buffer, GLenum target, size_t size, void *data,
			GLenum usage);
		void Bind() const;
		void Unbind() const;

		GLuint Name;
	private:
		VertexArray(const VertexArray&) { }
		VertexArray& operator=(const VertexArray&) { }
	};
}