#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <exception>

gfx::ShaderProgram::ShaderProgram()
{
	program = glCreateProgram();
}

gfx::ShaderProgram::ShaderProgram(std::vector<Shader *>& shaders)
{
	program = glCreateProgram();
	LinkProgram(shaders);
}

gfx::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(program);
}

void gfx::ShaderProgram::LinkProgram(std::vector<Shader *> &shaders)
{
	for (std::size_t i = 0; i < shaders.size(); i++)
	{
		glAttachShader(program, shaders[i]->Name);
	}
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		std::cerr << "Shader linking failed with this message:" << std::endl;
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;

		throw std::exception();
	}

	for (std::size_t i = 0; i < shaders.size(); i++)
	{
		glDetachShader(program, shaders[i]->Name);
	}
}

GLint gfx::ShaderProgram::GetUniformLocation(std::string name) const
{
	return glGetUniformLocation(program, name.c_str());
}

void gfx::ShaderProgram::Install() const
{
	glUseProgram(program);
}

void gfx::ShaderProgram::Uninstall() const
{
	glUseProgram(0);
}

gfx::ShaderProgram::Uniform gfx::ShaderProgram::GetUniform(std::string name) const
{
	Uniform u = { this, glGetUniformLocation(program, name.c_str()), name };
	return u;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(glm::vec2 value)
{
	warn(GL_FLOAT_VEC2, "vec2");
	program->Install();
	glUniform2fv(Location, 1, glm::value_ptr(value));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(glm::vec3 value)
{
	warn(GL_FLOAT_VEC3, "vec3");
	program->Install();
	glUniform3fv(Location, 1, glm::value_ptr(value));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(glm::vec4 value)
{
	warn(GL_FLOAT_VEC4, "vec4");
	program->Install();
	glUniform4fv(Location, 1, glm::value_ptr(value));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(glm::mat3 value)
{
	warn(GL_FLOAT_MAT3, "mat3");
	program->Install();
	glUniformMatrix3fv(Location, 1, GL_FALSE, glm::value_ptr(value));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(glm::mat4 value)
{
	warn(GL_FLOAT_MAT4, "mat4");
	program->Install();
	glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(value));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(
	const std::vector<glm::vec2>& values)
{
	warn(GL_FLOAT_VEC2, "vec2", values.size());
	program->Install();
	glUniform2fv(Location, values.size(), glm::value_ptr(values[0]));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(
	const std::vector<glm::vec3>& values)
{
	warn(GL_FLOAT_VEC3, "vec3", values.size());
	program->Install();
	glUniform3fv(Location, values.size(), glm::value_ptr(values[0]));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(
	const std::vector<glm::vec4>& values)
{
	warn(GL_FLOAT_VEC4, "vec4", values.size());
	program->Install();
	glUniform4fv(Location, values.size(), glm::value_ptr(values[0]));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(
	const std::vector<glm::mat3>& values)
{
	warn(GL_FLOAT_MAT3, "mat3", values.size());
	program->Install();
	glUniformMatrix3fv(
		Location, values.size(), GL_FALSE, glm::value_ptr(values[0]));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(
	const std::vector<glm::mat4>& values)
{
	warn(GL_FLOAT_MAT4, "mat4", values.size());
	program->Install();
	glUniformMatrix4fv(
		Location, values.size(), GL_FALSE, glm::value_ptr(values[0]));
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(GLfloat value)
{
	warn(GL_FLOAT, "float");
	program->Install();
	glUniform1f(Location, value);
	program->Uninstall();
	return *this;
}

gfx::ShaderProgram::Uniform& gfx::ShaderProgram::Uniform::Set(GLint value)
{
	/// \TODO type check
	program->Install();
	glUniform1i(Location, value);
	program->Uninstall();
	return *this;
}

void gfx::ShaderProgram::Uniform::warn(GLenum sym, std::string keyword, int num)
{
	const GLchar *name = Name.c_str();
	GLuint index;
	glGetUniformIndices(program->program, 1, &name, &index);
	if (index == GL_INVALID_INDEX)
	{
		std::cerr << "Warning: Uniform variable `" << Name << "` "
			"is not an active uniform." << std::endl;
		return;
	}
	GLint size;
	GLenum type;
	glGetActiveUniform(
		program->program, index, 0, nullptr, &size, &type, nullptr);

}