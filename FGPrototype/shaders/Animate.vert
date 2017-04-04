#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aBoneIndices;
layout (location = 3) in vec4 aBoneWeights;

out vec3 vPosition;
out vec3 vTransformedNormal;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uBoneMatrices[21];

void main()
{
	mat4 bone_transform =
		uBoneMatrices[int(aBoneIndices.x)] * aBoneWeights.x +
		uBoneMatrices[int(aBoneIndices.y)] * aBoneWeights.y +
		uBoneMatrices[int(aBoneIndices.z)] * aBoneWeights.z +
		uBoneMatrices[int(aBoneIndices.w)] * aBoneWeights.w;
	mat4 transform = uMVMatrix * bone_transform;
	vec4 camera_position = transform * vec4(aPosition, 1);
	mat3 normalMatrix = mat3(transpose(inverse(transform)));
	vPosition = camera_position.xyz;
	vTransformedNormal = normalMatrix * aNormal;
	gl_Position = uPMatrix * camera_position;
}
