#include "VertexArray.h"

gfx::VertexArray::VertexArray()
{
	glGenVertexArrays(1, &Name);
}

gfx::VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &Name);
}

void gfx::VertexArray::SetBufferData(
	GLuint buffer, GLenum target, size_t size, void *data, GLenum usage)
{
	Bind();
	glBindBuffer(target, buffer);
	glBufferData(target, size, data, usage);
	Unbind();
}

void gfx::VertexArray::Bind() const
{
	glBindVertexArray(Name);
}

void gfx::VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
