#pragma once

#include "Mesh.h"
#include "BoneAnimation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include <vector>

namespace gfx
{
	class ModelImporter
	{
	public:
		bool ImportModel(
			std::string file,
			std::vector<Mesh *>& meshes);
		Animation LoadAnimation(std::string file, Mesh *mesh);
	private:
		void PrintModelInfo(const aiScene *model);
		void ExtractTriangles(
			const aiScene *model,
			std::vector<Mesh *>& meshes);
		void ExtractAnimations(
			const aiScene *model,
			Mesh *mesh,
			std::vector<Animation>& anims);
		Animation LoadAnimation(const aiAnimation *anim, Mesh *mesh);

		static const int frame_rate = 60;
	};
}