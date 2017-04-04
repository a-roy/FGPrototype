#include "Mesh.h"
#include <glm/gtc/constants.hpp>

gfx::Mesh::Mesh(int vertexBuffers, int numVerts, int numFaces) :
	IBO(&VAO, numFaces),
	NumVerts(numVerts),
	NumTris(numFaces)
{
	for (int i = 0; i < vertexBuffers; i++)
	{
		VBOs.push_back(FloatBuffer(&VAO, 1, numVerts));
	}
	VAO.Bind();
	for (int i = 0; i < vertexBuffers; i++)
	{
		glEnableVertexAttribArray(i);
	}
	VAO.Unbind();
}

void gfx::Mesh::Draw() const
{
	VAO.Bind();
	glDrawElements(GL_TRIANGLES, NumTris * 3, GL_UNSIGNED_INT, (void *)0);
	VAO.Unbind();
}

void gfx::Mesh::SetVertexData(
	GLuint attribute, GLfloat *data, int itemSize)
{
	VBOs[attribute].Reinit(&VAO, itemSize, NumVerts);
	VBOs[attribute].SetData(data);
	VBOs[attribute].VertexAttribPointer(attribute);
}

void gfx::Mesh::SetIndexData(unsigned int *iboData)
{
	IBO.Reinit(&VAO, NumTris);
	IBO.SetData(iboData);
	IBO.BindBuffer();
}