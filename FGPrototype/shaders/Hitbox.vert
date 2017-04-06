#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec4 aColor;

out vec4 vColor;

uniform mat4 uMVPMatrix;

void main()
{
	vColor = aColor;
	gl_Position = uMVPMatrix * vec4(aPosition, 0, 1);
}