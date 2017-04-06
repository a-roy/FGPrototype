#include "GraphicsSystem.h"

void GraphicsSystem::Render(EntityList& entities)
{
	for (int i = 0; i < num_entities; i++)
	{
		if (entities.enabled[i][IVisible]
			&& entities.enabled[i][IPlacement]
			&& entities.enabled[i][IAction])
		{
			glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			float xy_scale = 1.f / 250.f;
			RenderModel(
				entities.visible[i].Model,
				entities.action[i].Current,
				entities.action[i].Time,
				entities.placement[i].O,
				entities.placement[i].X * xy_scale,
				-entities.placement[i].Y * xy_scale,
				1.f);
		}
	}
}

void GraphicsSystem::RenderModel(
	int model, int animation, int time, int o, float xpos, float ypos, float zoom)
{
	gfx::Mesh *mesh = Models[model];
	gfx::Animation& anim = Animations[model][animation];
	gfx::AnimTrack track = ComputeTrack(anim, time);
	std::vector<glm::mat4> bone_matrices(mesh->Bones.size());
	ComputeBoneMatrices(-1, mesh, track, bone_matrices);

	// Conversion from Blender's coordinate system
	glm::mat4 coords = glm::mat4(
		0.f, 0.f, -o, 0.f,
		-o, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f);
	float xy_scale = 1.f / 250.f;
	glm::mat4 scale = glm::scale(glm::vec3(zoom));
	glm::mat4 rotate(1.f);
	glm::mat4 translate = glm::translate(
		glm::vec3(xpos, ypos, 0.f));
	glm::mat4 mMatrix = translate * rotate * scale * coords;
	glm::mat4 vMatrix = glm::lookAt(
		glm::vec3(0.f, 15, 20),
		glm::vec3(0, 5, 0),
		glm::vec3(0, 1, 0));
	glm::mat4 mvMatrix = vMatrix * mMatrix;
	glm::mat4 pMatrix = glm::perspective(
		glm::radians(45.0f), 16.0f / 9.0f, 1.f, 100.0f);
	glm::vec3 diffuseColor(0.5, 0.4, 0.1);
	CharProgram->GetUniform("uMVMatrix").Set(mvMatrix);
	CharProgram->GetUniform("uPMatrix").Set(pMatrix);
	CharProgram->GetUniform("uBoneMatrices").Set(bone_matrices);
	CharProgram->GetUniform("uDiffuseColor").Set(diffuseColor);
	CharProgram->GetUniform("uSpecularHighlight").Set(GL_FALSE);
	CharProgram->Install();
	mesh->Draw();
	CharProgram->Uninstall();
}

gfx::AnimTrack GraphicsSystem::ComputeTrack(gfx::Animation& anim, int time) const
{
	gfx::AnimTrack track(anim.size());
	track.AnimSet = anim;
	for (int i = 0, n = anim.size(); i < n; i++)
	{
		anim[i].Interpolate(
				time,
				&track.ScaleKeys[i],
				&track.RotateKeys[i],
				&track.TranslateKeys[i]);
	}
	return track;
}

void GraphicsSystem::ComputeBoneMatrices(
	int i, gfx::Mesh *mesh, gfx::AnimTrack& track, std::vector<glm::mat4>& matrices)
{
	if (i < 0)
	{
		for (std::size_t j = 0; j < mesh->Bones.size(); j++)
		{
			if (mesh->Bones[j].Parent < 0)
			{
				matrices[j] = glm::mat4(1.f);
				ComputeBoneMatrices(j, mesh, track, matrices);
			}
		}
	}
	else
	{
		glm::mat4 srt =
			glm::translate(track.TranslateKeys[i]) *
			glm::mat4_cast(track.RotateKeys[i]) *
			glm::scale(track.ScaleKeys[i]);
		matrices[i] = matrices[i] * srt;

		for (std::size_t j = 0; j < mesh->Bones[i].Children.size(); j++)
		{
			int child = mesh->Bones[i].Children[j];
			matrices[child] = matrices[i];
			ComputeBoneMatrices(child, mesh, track, matrices);
		}
		matrices[i] = matrices[i] * mesh->Bones[i].OffsetMatrix;
	}
}
