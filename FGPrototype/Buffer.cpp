#include "Buffer.h"

gfx::FloatBuffer::FloatBuffer(
	VertexArray *vao, unsigned int itemSize, unsigned int numItems) :
	Buffer<GLfloat>(vao, GL_ARRAY_BUFFER, GL_STATIC_DRAW, itemSize, numItems)
{ }

void gfx::FloatBuffer::Reinit(VertexArray *vao, unsigned int itemSize,
	unsigned int numItems)
{
	Buffer::Reinit(
		vao, GL_ARRAY_BUFFER, GL_STATIC_DRAW, itemSize, numItems);
}

void gfx::FloatBuffer::VertexAttribPointer(GLuint attribute, size_t stride)
{
	BindBuffer();
	VAO->Bind();
	glVertexAttribPointer(
		attribute,
		ItemSize,
		GL_FLOAT,
		GL_FALSE,
		stride,
		(void *)0);
	glEnableVertexAttribArray(attribute);
	VAO->Unbind();
	glBindBuffer(Target, 0);
}

gfx::ElementBuffer::ElementBuffer(VertexArray *vao, int numItems) :
	Buffer<unsigned int>(
		vao, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 3, numItems)
{ }

void gfx::ElementBuffer::Reinit(VertexArray *vao, int numItems)
{
	Buffer::Reinit(
		vao, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 3, numItems);
}