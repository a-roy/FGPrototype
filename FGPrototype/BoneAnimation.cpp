#include "BoneAnimation.h"

void gfx::BoneAnimation::Interpolate(
	int time,
	glm::vec3 *scale, glm::quat *rotate, glm::vec3 *trans) const
{
	if (scale != NULL)
	{
		if (ScaleKeys.empty())
			*scale = glm::vec3(1, 1, 1);
		else
			*scale = Lerp(ScaleKeys, time);
	}
	if (rotate != NULL)
	{
		if (RotateKeys.empty())
			*rotate = glm::quat(1, 0, 0, 0);
		else
			*rotate = Slerp(RotateKeys, time);
	}
	if (trans != NULL)
	{
		if (TranslateKeys.empty())
			*trans = glm::vec3(0, 0, 0);
		else
			*trans = Lerp(TranslateKeys, time);
	}
}

glm::mat4 gfx::BoneAnimation::Interpolate(int time) const
{
	glm::mat4 scale = ScaleKeys.empty() ? glm::mat4(1.f) :
		glm::scale(Lerp(ScaleKeys, time));
	glm::mat4 rotate = RotateKeys.empty() ? glm::mat4(1.f) :
		glm::mat4_cast(Slerp(RotateKeys, time));
	glm::mat4 translate = TranslateKeys.empty() ? glm::mat4(1.f) :
		glm::translate(Lerp(TranslateKeys, time));
	return translate * rotate * scale;
}

glm::mat4 gfx::BoneAnimation::Interpolate(
	const BoneAnimation *prevAnim, int prevTime,
	const BoneAnimation *newAnim, int newTime)
{
	glm::mat4 scale, rotate, translate;
	int nextKey;
	nextKey = FirstNonzeroKey<glm::vec3>(newAnim->ScaleKeys);
	if (nextKey >= 0 || newTime >= newAnim->ScaleKeys[nextKey].Time)
	{
		scale = glm::scale(Lerp(newAnim->ScaleKeys, newTime));
	}
	else
	{
		scale = glm::scale(glm::mix(
			Lerp(prevAnim->ScaleKeys, prevTime),
			newAnim->ScaleKeys[nextKey].Value,
			(float)newTime / newAnim->ScaleKeys[nextKey].Time));
	}
	nextKey = FirstNonzeroKey<glm::quat>(newAnim->RotateKeys);
	if (nextKey >= 0 || newTime >= newAnim->RotateKeys[nextKey].Time)
	{
		rotate = glm::mat4_cast(Slerp(newAnim->RotateKeys, newTime));
	}
	else
	{
		rotate = glm::mat4_cast(glm::slerp(
			Slerp(prevAnim->RotateKeys, prevTime),
			newAnim->RotateKeys[nextKey].Value,
			(float)newTime / newAnim->ScaleKeys[nextKey].Time));
	}
	nextKey = FirstNonzeroKey<glm::vec3>(newAnim->TranslateKeys);
	if (nextKey >= 0 || newTime >= newAnim->TranslateKeys[nextKey].Time)
	{
		translate = glm::translate(Lerp(newAnim->TranslateKeys, newTime));
	}
	else
	{
		translate = glm::translate(glm::mix(
			Lerp(prevAnim->TranslateKeys, prevTime),
			newAnim->TranslateKeys[nextKey].Value,
			(float)newTime / newAnim->TranslateKeys[nextKey].Time));
	}
	return translate * rotate * scale;
}

template<typename T>
int gfx::BoneAnimation::FirstNonzeroKey(const std::vector<KeyFrame<T>>& keys)
{
	for (unsigned int i = 0; i < keys.size(); i++)
	{
		if (keys[i].Time > 0)
		{
			return i;
		}
	}
	return -1;
}

template<typename T>
int gfx::BoneAnimation::BinarySearch(const std::vector<KeyFrame<T>>& keys, int time)
{
	int lower = 0;
	int upper = keys.size() - 1;
	if (time > keys[upper].Time)
		return upper;
	while (upper - lower > 1)
	{
		int i = (lower + upper + 1) / 2;
		if (time < keys[i].Time)
		{
			upper = i;
		}
		else if (time > keys[i].Time)
		{
			lower = i;
		}
		else
		{
			return i;
		}
	}
	return lower;
}

glm::vec3 gfx::BoneAnimation::Lerp(const std::vector<KeyFrame<glm::vec3>>& keys, int time)
{
	int prev = BinarySearch<glm::vec3>(keys, time);
	int next = prev + 1;
	if (keys[prev].Time >= time || next == keys.size())
	{
		return keys[prev].Value;
	}
	else
	{
		return glm::mix(keys[prev].Value, keys[next].Value,
			(float)(time - keys[prev].Time) /
			(keys[next].Time - keys[prev].Time));
	}
}

glm::quat gfx::BoneAnimation::Slerp(const std::vector<KeyFrame<glm::quat>>& keys, int time)
{
	int prev = BinarySearch<glm::quat>(keys, time);
	int next = prev + 1;
	if (keys[prev].Time >= time || next == keys.size())
	{
		return keys[prev].Value;
	}
	else
	{
		return glm::slerp(keys[prev].Value, keys[next].Value,
			(float)(time - keys[prev].Time) /
			(keys[next].Time - keys[prev].Time));
	}
}