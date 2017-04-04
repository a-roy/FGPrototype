//! \file
//! Mesh class declaration
#pragma once

#include "Buffer.h"
#include "Bone.h"
#include <vector>
#include <string>

//! Represents a 3D mesh
namespace gfx
{
	struct Mesh
	{
	public:
		VertexArray VAO;
		std::vector<FloatBuffer> VBOs;
		ElementBuffer IBO;
		std::vector<Bone> Bones;
		std::map<std::string, int> BoneNames;

		int NumVerts;
		int NumTris;

		Mesh(int vertexBuffers, int numVerts, int numFaces);
		void Draw() const;
		void SetVertexData(
			GLuint attribute, GLfloat *data, int itemSize);
		void SetIndexData(unsigned int *iboData);

		//std::string DiffuseMap;
		//std::string NormalMap;
	};
}