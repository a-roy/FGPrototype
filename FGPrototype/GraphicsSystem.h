#pragma once
#include "Component.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "BoneAnimation.h"
#include "AnimTrack.h"

class GraphicsSystem
{
public:
	void Render(EntityList& entities);
	gfx::AnimTrack ComputeTrack(gfx::Animation& anim, int time) const;
	void ComputeBoneMatrices(
		int i, gfx::Mesh *mesh, gfx::AnimTrack& track,
		std::vector<glm::mat4>& matrices);

	gfx::ShaderProgram *CharProgram;
	gfx::ShaderProgram *TextProgram;
	std::vector<gfx::Mesh *> Models;
	std::vector<std::vector<gfx::Animation> > Animations;
};
