#version 330 core

in vec3 vPosition;
in vec3 vTransformedNormal;

out vec4 fColor;

uniform vec3 uDiffuseColor;
uniform bool uSpecularHighlight;

void main()
{
	float shininess = 100.0;
	vec3 lightPos = vec3(0, 10, 0);
	vec3 ambientColor = vec3(0.1, 0.1, 0.1);
	vec3 specularColor = vec3(0.8, 0.8, 0.8);
	vec3 diffuseColor = uDiffuseColor;

	vec3 lightDirection = normalize(lightPos - vPosition.xyz);
	vec3 normal = normalize(vTransformedNormal);

	float specularLightWeighting = 0.0;
	if (uSpecularHighlight)
	{
		vec3 eyeDirection = normalize(-vPosition.xyz);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		specularLightWeighting =
			pow(max(dot(reflectionDirection, eyeDirection), 0.0), shininess);
	}

	float diffuseLightWeighting = max(dot(normal, lightDirection), 0.0);
	vec3 lightWeighting = ambientColor
		+ specularColor * specularLightWeighting
		+ diffuseColor * diffuseLightWeighting;

	vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
	fColor = vec4(color.rgb * lightWeighting, color.a);
}
