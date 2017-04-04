//! \file
//! Bone class declaration
#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>

namespace gfx
{
	//! Represents a bone in a mesh
	struct Bone
	{
	public:
		int Parent;
		//! Indices of child bones
		//!
		//! \internal We use ints instead of pointers because our Mesh object has Bones organized in a
		//!           vector, and we need bones to have vector indices during rendering.
		std::vector<int> Children;
		//! Transforms from mesh space into bone space in bind pose; AKA Inverse Bind Matrix.
		glm::mat4 OffsetMatrix;
		//! The vertices that this bone influences.
		std::vector<unsigned int> Vertices;
		//! This bone's weight for each vertex that it influences.
		std::vector<float> Weights;
	};
}