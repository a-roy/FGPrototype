#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>

namespace gfx
{
	class Shader
	{
	public:
		GLuint Name;
		GLenum ShaderType;
		std::string Path;

		Shader(std::string path, GLenum shaderType);
		Shader(std::string path);
		~Shader();
		Shader(const Shader& other);
		Shader& operator=(const Shader& other);
		void Load(std::string path);
		void Compile();
		static void ReadSource(const char *fname, std::vector<char> &buffer);
	};
}