#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
//#include "Frame.h"
#include <vector>
#include <string>
#include <map>

namespace gfx
{
	class ShaderProgram
	{
	public:
		struct Uniform
		{
			const ShaderProgram *program;
			GLint Location;
			std::string Name;
			Uniform& Set(glm::vec2 value);
			Uniform& Set(glm::vec3 value);
			Uniform& Set(glm::vec4 value);
			Uniform& Set(glm::mat3 value);
			Uniform& Set(glm::mat4 value);
			Uniform& Set(const std::vector<glm::vec2>& values);
			Uniform& Set(const std::vector<glm::vec3>& values);
			Uniform& Set(const std::vector<glm::vec4>& values);
			Uniform& Set(const std::vector<glm::mat3>& values);
			Uniform& Set(const std::vector<glm::mat4>& values);
			Uniform& Set(GLfloat value);
			Uniform& Set(GLint value);
			void warn(GLenum sym, std::string keyword, int num = 1);
		};
		GLuint program;

		ShaderProgram();
		ShaderProgram(std::vector<Shader *>& shaders);
		~ShaderProgram();
		void LinkProgram(std::vector<Shader *> &shaders);
		GLint GetUniformLocation(std::string name) const;
		Uniform GetUniform(std::string name) const;
		void Install() const;
		void Uninstall() const;
	private:
		ShaderProgram(const ShaderProgram&) { }
		ShaderProgram& operator=(const ShaderProgram&) { }
	};
}