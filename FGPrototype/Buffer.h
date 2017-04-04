#pragma once

#include <glad/glad.h>
#include "VertexArray.h"
#include <cstring>

namespace gfx
{

	template <class T>
	class Buffer
	{
	public:
		GLuint Name;
		VertexArray *VAO;
		GLenum Target;
		GLenum Usage;
		unsigned int ItemSize;
		unsigned int NumItems;
		T *Data;

		Buffer(
			VertexArray *vao,
			GLenum target,
			GLenum usage,
			unsigned int itemSize,
			unsigned int numItems);
		~Buffer();
		//! Substitute for move assignment
		void Reinit(VertexArray *vao, GLenum target, GLenum usage,
			unsigned int itemSize, unsigned int numItems);

		virtual void SetData(T *data, bool copy = true);
		virtual void BindBuffer();
		virtual void VertexAttribPointer(GLuint attribute, size_t stride = 0)
		{ }

	private:
		//Buffer(const Buffer<T>&) { }
		Buffer<T>& operator=(const Buffer<T>&) { }
		bool managed_data;
	};
#include "Buffer.inl"

	class FloatBuffer : public Buffer<GLfloat>
	{
	public:
		FloatBuffer(
			VertexArray *vao,
			unsigned int itemSize,
			unsigned int numItems);
		void Reinit(VertexArray *vao, unsigned int itemSize,
			unsigned int numItems);

		void VertexAttribPointer(GLuint attribute, size_t stride = 0);
	};

	class ElementBuffer : public Buffer<unsigned int>
	{
	public:
		ElementBuffer(VertexArray *vao, int numItems);
		void Reinit(VertexArray *vao, int numItems);
	};
}