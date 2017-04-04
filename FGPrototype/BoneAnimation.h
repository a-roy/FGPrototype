//! \file
//! BoneAnimation, KeyFrame class declarations
#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

namespace gfx
{
	template<typename T>
	struct KeyFrame
	{
		int Time;
		T Value;
	};

	//! Describes a full animation for a single bone.
	class BoneAnimation
	{
	public:
		//! Keyframes indicating bone scaling
		//!
		//! \warning Key containers should always be sorted in ascending order with no duplicate times.
		std::vector<KeyFrame<glm::vec3> > ScaleKeys;
		//! Keyframes indicating bone rotation
		//!
		//! \copydetails ::ScaleKeys
		std::vector<KeyFrame<glm::quat> > RotateKeys;
		//! Keyframes indicating bone translation
		//!
		//! \copydetails ::ScaleKeys
		std::vector<KeyFrame<glm::vec3> > TranslateKeys;
		//!
		void Interpolate(
			int time,
			glm::vec3 *scale, glm::quat *rotate, glm::vec3 *trans) const;
			//! Interpolates key frames to a transformation matrix for a given frame.
			//!
			//! Requires all key frames to be sorted.
		glm::mat4 Interpolate(int time) const;
		//! Interpolates a key frame between two animations.
		//!
		//! If the new animation has already reached a nonzero key in any sub-channel, it will simply
		//! interpolate with itself.
		static glm::mat4 Interpolate(const BoneAnimation* prevAnim, int prevTime, const BoneAnimation* newAnim, int newTime);
	private:
		//! Returns the index of the lower or upper bound of the searched time.
		//!
		//! \param keys  The vector to search, which must be sorted.
		//! \param time  The frame to search for.
		template<typename T>
		static int BinarySearch(const std::vector<KeyFrame<T> >& keys, int time);
		//! Finds the first nonzero time of a key frame in an animation sub-channel.
		//!
		//! \param keys
		//! \returns The index of a key frame, or -1 if not found.
		template<typename T>
		static int FirstNonzeroKey(const std::vector<KeyFrame<T> >& keys);
		static glm::vec3 Lerp(const std::vector<KeyFrame<glm::vec3> >& keys, int time);
		static glm::quat Slerp(const std::vector<KeyFrame<glm::quat> >& keys, int time);
	};

	typedef std::vector<BoneAnimation> Animation;
}