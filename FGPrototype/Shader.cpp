#include "Shader.h"
#include <fstream>
#include <iostream>
#include <exception>

gfx::Shader::Shader(std::string path, GLenum shaderType) :
	Path(path), ShaderType(shaderType)
{
	Name = glCreateShader(shaderType);
	Load(path);
	Compile();
}

gfx::Shader::Shader(std::string path) :
	Path(path)
{
	std::string extension = path.substr(path.length() - 4, 4);
	if (extension == "vert")
		ShaderType = GL_VERTEX_SHADER;
	else if (extension == "tesc")
		ShaderType = GL_TESS_CONTROL_SHADER;
	else if (extension == "tese")
		ShaderType = GL_TESS_EVALUATION_SHADER;
	else if (extension == "geom")
		ShaderType = GL_GEOMETRY_SHADER;
	else if (extension == "frag")
		ShaderType = GL_FRAGMENT_SHADER;

	Name = glCreateShader(ShaderType);
	Load(path);
	Compile();
}

gfx::Shader::~Shader()
{
	glDeleteShader(Name);
}

gfx::Shader::Shader(const Shader& other) :
	Name(0)
{
	*this = other;
}

gfx::Shader& gfx::Shader::operator=(const Shader& other)
{
	GLint length;
	glGetShaderiv(other.Name, GL_SHADER_SOURCE_LENGTH, &length);
	std::vector<char> buffer(length);
	char *src = buffer.data();
	glGetShaderSource(other.Name, length, NULL, src);

	glDeleteShader(Name);
	Name = glCreateShader(other.ShaderType);
	ShaderType = other.ShaderType;
	Path = other.Path;
	glShaderSource(Name, 1, &src, NULL);
	Compile();

	return *this;
}

void gfx::Shader::Load(std::string path)
{
	std::vector<char> buffer;
	ReadSource(path.c_str(), buffer);
	const char *src = buffer.data();
	glShaderSource(Name, 1, &src, NULL);
}

void gfx::Shader::Compile()
{
	// Compile the shader
	glCompileShader(Name);
	// Check the result of the compilation
	GLint isCompiled;
	glGetShaderiv(Name, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		std::cerr << "Shader compilation (" << Path <<
			") failed with this message:" << std::endl;
		GLint maxLength = 0;
		glGetShaderiv(Name, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(Name, maxLength, &maxLength, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;

		throw std::exception();
	}
}

void gfx::Shader::ReadSource(const char *fname, std::vector<char> &buffer)
{
	std::ifstream in;
	in.open(fname, std::ios::binary);

	if (in.is_open())
	{
		// Get the number of bytes stored in this file
		in.seekg(0, std::ios::end);
		size_t length = (size_t)in.tellg();

		// Go to start of the file
		in.seekg(0, std::ios::beg);

		// Read the content of the file in a buffer
		buffer.resize(length + 1);
		in.read(&buffer[0], length);
		in.close();
		// Add a valid C - string end
		buffer[length] = '\0';
	}
	else
	{
		std::cerr << "Unable to open " << fname << std::endl;
		throw std::exception();
	}
}