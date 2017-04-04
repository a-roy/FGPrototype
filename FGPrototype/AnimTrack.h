#pragma once

namespace gfx
{
	//! Describes an interpolated pose for a bone hierarchy
	struct AnimTrack
	{
		AnimTrack(int n = 0) :
			ScaleKeys(n, glm::vec3(1.f)), RotateKeys(n), TranslateKeys(n), size(n)
		{ }
		gfx::Animation AnimSet;
		std::vector<glm::vec3> ScaleKeys;
		std::vector<glm::quat> RotateKeys;
		std::vector<glm::vec3> TranslateKeys;
		int size;
	};
}