#include "ModelImporter.h"
#include <map>

bool gfx::ModelImporter::ImportModel(
	std::string file,
	std::vector<Mesh *>& meshes)
{
	std::cout << " Loading model: " << file << std::endl;

	Assimp::Importer importer;

	const aiScene *model = importer.ReadFile(file,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);

	if (!model)
	{
		std::cerr << "Error importing '" << file
			<< "': " << importer.GetErrorString() << std::endl;
		return false;
	}

	PrintModelInfo(model);

	/* Create the VAOs for the model */
	ExtractTriangles(model, meshes);

	return true;
}

void gfx::ModelImporter::PrintModelInfo(const aiScene *model)
{
	std::cout << " |-Materials: " << model->mNumMaterials << std::endl;
	std::cout << " |-Textures: " << model->mNumTextures << std::endl;
	std::cout << " |-Cameras: " << model->mNumCameras << std::endl;
	std::cout << " |-Animations: " << model->mNumAnimations << std::endl;
	std::cout;
}

void gfx::ModelImporter::ExtractTriangles(
	const aiScene *model,
	std::vector<Mesh *> &meshes)
{
	std::cout << " |-Meshes: " << model->mNumMeshes << std::endl;
	for (unsigned int i = 0; i < model->mNumMeshes; i++)
	{
		aiMesh *mesh = model->mMeshes[i];

		std::cout << "   |-Mesh " << i << std::endl;
		std::cout << "     |-Verts: " << mesh->mNumVertices << std::endl;

		GLfloat *meshVertexData = new GLfloat[mesh->mNumVertices * 3];

		GLfloat *meshNormalsData;
		bool hasNormals = mesh->HasNormals();
		if (hasNormals)
			meshNormalsData = new GLfloat[mesh->mNumVertices * 3];

		GLfloat *meshTexCoordsData;
		bool hasTexCoords = mesh->HasTextureCoords(0);
		if (hasTexCoords)
			meshTexCoordsData = new GLfloat[mesh->mNumVertices * 2];

		GLfloat *meshBoneIndexData;
		GLfloat *meshBoneWeightData;
		bool hasBones = mesh->HasBones();
		if (hasBones)
		{
			meshBoneIndexData = new GLfloat[mesh->mNumVertices * 4];
			meshBoneWeightData = new GLfloat[mesh->mNumVertices * 4];
			for (unsigned int i = 0; i < mesh->mNumVertices * 4; i++)
			{
				meshBoneIndexData[i] = 0;
				meshBoneWeightData[i] = 0.f;
			}
		}

		/* Direct copy VBO from mesh */
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{
			meshVertexData[(j * 3) + 0] = mesh->mVertices[j].x;
			meshVertexData[(j * 3) + 1] = mesh->mVertices[j].y;
			meshVertexData[(j * 3) + 2] = mesh->mVertices[j].z;

			if (hasNormals)
			{
				meshNormalsData[(j * 3) + 0] = mesh->mNormals[j].x;
				meshNormalsData[(j * 3) + 1] = mesh->mNormals[j].y;
				meshNormalsData[(j * 3) + 2] = mesh->mNormals[j].z;
			}

			if (hasTexCoords)
			{
				meshTexCoordsData[(j * 2) + 0] = mesh->mTextureCoords[0][j].x;
				meshTexCoordsData[(j * 2) + 1] = mesh->mTextureCoords[0][j].y;
			}
		}

		std::vector<Bone> bones(mesh->mNumBones);
		std::map<std::string, int> bone_names;
		for (unsigned int j = 0; j < mesh->mNumBones; j++)
		{
			bone_names[mesh->mBones[j]->mName.C_Str()] = j;
		}

		aiNode *rootNode = model->mRootNode;
		for (unsigned int j = 0; j < mesh->mNumBones; j++)
		{
			aiBone *b = mesh->mBones[j];
			aiNode *n = rootNode->FindNode(b->mName);
			aiMatrix4x4& m = b->mOffsetMatrix;
			if (n->mParent == NULL)
			{
				bones[j].Parent = -1;
			}
			else
			{
				const char *parent_name = n->mParent->mName.C_Str();
				if (bone_names.find(parent_name) == bone_names.end())
					bones[j].Parent = -1;
				else
					bones[j].Parent = bone_names[parent_name];
			}
			for (unsigned int k = 0; k < n->mNumChildren; k++)
			{
				const char *bone_name = n->mChildren[k]->mName.C_Str();
				if (bone_names.find(bone_name) == bone_names.end())
					continue;
				bones[j].Children.push_back(bone_names[bone_name]);
			}
			bones[j].OffsetMatrix = glm::mat4(
				m.a1, m.b1, m.c1, m.d1,
				m.a2, m.b2, m.c2, m.d2,
				m.a3, m.b3, m.c3, m.d3,
				m.a4, m.b4, m.c4, m.d4);
			for (unsigned int k = 0; k < b->mNumWeights; k++)
			{
				aiVertexWeight& w = b->mWeights[k];
				bones[j].Vertices.push_back(w.mVertexId);
				bones[j].Weights.push_back(w.mWeight);
				for (int x = 0; x < 4; x++)
				{
					int i0 = w.mVertexId * 4 + x;
					if (w.mWeight > meshBoneWeightData[i0])
					{
						for (int y = 3; y > x; y--)
						{
							int i1 = w.mVertexId * 4 + y;
							int i2 = i1 - 1;
							meshBoneIndexData[i1] = meshBoneIndexData[i2];
							meshBoneWeightData[i1] = meshBoneWeightData[i2];
						}
						meshBoneIndexData[i0] = (GLfloat)j;
						meshBoneWeightData[i0] = w.mWeight;
						break;
					}
				}
			}
		}

		/* Extract the VBI from faces */
		std::cout << "     |-Polys: " << mesh->mNumFaces << std::endl;
		unsigned int *meshIndexData = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			meshIndexData[(j * 3) + 0] = mesh->mFaces[j].mIndices[0];
			meshIndexData[(j * 3) + 1] = mesh->mFaces[j].mIndices[1];
			meshIndexData[(j * 3) + 2] = mesh->mFaces[j].mIndices[2];
		}

		/* Create a local mesh */
		int num_buffers = 1;
		int index = 0;
		int normal_index, texcoord_index, bone_index;
		if (hasNormals) normal_index = num_buffers++;
		if (hasTexCoords) texcoord_index = num_buffers++;
		if (hasBones)
		{
			bone_index = num_buffers;
			num_buffers += 2;
		}
		gfx::Mesh *resultMesh =
			new gfx::Mesh(num_buffers, mesh->mNumVertices, mesh->mNumFaces);
		resultMesh->SetVertexData(0, meshVertexData, 3);
		if (hasNormals)
			resultMesh->SetVertexData(normal_index, meshNormalsData, 3);
		if (hasTexCoords)
			resultMesh->SetVertexData(texcoord_index, meshTexCoordsData, 2);
		if (hasBones)
		{
			resultMesh->SetVertexData(bone_index, meshBoneIndexData, 4);
			resultMesh->SetVertexData(bone_index + 1, meshBoneWeightData, 4);
		}
		resultMesh->SetIndexData(meshIndexData);
		resultMesh->Bones = bones;
		resultMesh->BoneNames = bone_names;
		meshes.push_back(resultMesh);
	}
}

void gfx::ModelImporter::ExtractAnimations(
	const aiScene *model,
	Mesh *mesh,
	std::vector<Animation>& anims)
{
	anims.clear();
	anims.resize(model->mNumAnimations);
	for (unsigned int i = 0; i < model->mNumAnimations; i++)
	{
		aiAnimation *anim = model->mAnimations[i];
		anims[i].resize(mesh->BoneNames.size());
		//anims[i].resize(anim->mNumChannels);
		for (unsigned int j = 0; j < anim->mNumChannels; j++)
		{
			aiNodeAnim *channel = anim->mChannels[j];
			int index = mesh->BoneNames[channel->mNodeName.C_Str()];
			BoneAnimation& bone_anim = anims[i][index];
			bone_anim.TranslateKeys.resize(channel->mNumPositionKeys);
			for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
			{
				aiVectorKey& key = channel->mPositionKeys[k];
				KeyFrame<glm::vec3>& keyframe = bone_anim.TranslateKeys[k];
				keyframe.Time = (int)(key.mTime * 60);
				keyframe.Value = glm::vec3(
					key.mValue.x, key.mValue.y, key.mValue.z);
			}
			bone_anim.RotateKeys.resize(channel->mNumRotationKeys);
			for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
			{
				aiQuatKey& key = channel->mRotationKeys[k];
				KeyFrame<glm::quat>& keyframe = bone_anim.RotateKeys[k];
				keyframe.Time = (int)(key.mTime * 60);
				keyframe.Value = glm::quat(
					key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
			}
			bone_anim.ScaleKeys.resize(channel->mNumScalingKeys);
			for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
			{
				aiVectorKey& key = channel->mScalingKeys[k];
				KeyFrame<glm::vec3>& keyframe = bone_anim.ScaleKeys[k];
				keyframe.Time = (int)(key.mTime * 60);
				keyframe.Value = glm::vec3(
					key.mValue.x, key.mValue.y, key.mValue.z);
			}
		}
	}
}

gfx::Animation gfx::ModelImporter::LoadAnimation(
	std::string file, Mesh *mesh)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, 0U);
	if (scene == NULL)
	{
		std::cerr << "Error importing '" << file
			<< "': " << importer.GetErrorString() << std::endl;
		return Animation();
	}
	const aiAnimation* anim = scene->mAnimations[0];
	return LoadAnimation(anim, mesh);
}

gfx::Animation gfx::ModelImporter::LoadAnimation(
	const aiAnimation *anim, Mesh *mesh)
{
	std::vector<BoneAnimation> myAnimation;
	for (unsigned int b = 0; b < anim->mNumChannels; b++)
	{
		aiNodeAnim* channel = anim->mChannels[b];
		BoneAnimation myChannel;
		std::map<std::string, int>& boneIDs = mesh->BoneNames;
		if (boneIDs.find(channel->mNodeName.C_Str()) == boneIDs.end())
		{
			continue;
		}
		unsigned int id = boneIDs[channel->mNodeName.C_Str()];
		for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
		{
			aiVectorKey key = channel->mScalingKeys[k];
			KeyFrame<glm::vec3> myKey;
			//myKey.Time = (int)key.mTime;
			myKey.Time = (int)(frame_rate * key.mTime); // for DAE file
			myKey.Value = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
			myChannel.ScaleKeys.push_back(myKey);
		}
		for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
		{
			aiQuatKey key = channel->mRotationKeys[k];
			KeyFrame<glm::quat> myKey;
			//myKey.Time = (int)key.mTime;
			myKey.Time = (int)(frame_rate * key.mTime); // for DAE file
			myKey.Value = glm::quat(
				key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
			myChannel.RotateKeys.push_back(myKey);
		}
		for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
		{
			aiVectorKey key = channel->mPositionKeys[k];
			KeyFrame<glm::vec3> myKey;
			//myKey.Time = (int)key.mTime;
			myKey.Time = (int)(frame_rate * key.mTime); // for DAE file
			myKey.Value = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
			myChannel.TranslateKeys.push_back(myKey);
		}
		if (id >= myAnimation.size())
		{
			myAnimation.resize(id + 1);
		}
		myAnimation[id] = myChannel;
	}
	return myAnimation;
}
