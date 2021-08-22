#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

void Shader::loadShaderFromFile(const char* vShaderPath, const char* fShaderPath)
{
	std::string vshader_source = loadShaderSourceFromFile(vShaderPath);
	std::string fshader_source = loadShaderSourceFromFile(fShaderPath);

	compile(vshader_source.c_str(), fshader_source.c_str());
}

std::string Shader::loadShaderSourceFromFile(std::string path)
{
	std::string txt;

	try
	{
		std::ifstream fstream(path);

		std::stringstream sstream;

		sstream << fstream.rdbuf();

		fstream.close();

		txt = sstream.str();
	}
	catch (std::exception e)
	{
		std::cerr << "| ERROR::SHADER: LOAD-TIME error: Type: " << e.what() << std::endl;
	}

	return txt;
}

void Shader::use()
{
	glUseProgram(this->ID);
}

void Shader::compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	unsigned int sVertex, sFragment, gShader;
	// vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	// fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	// if geometry shader source code is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, sVertex);
	glAttachShader(ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(ID, gShader);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(gShader);
}

void Shader::setFloat(const char* name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setInteger(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setVector2f(const char* name, float x, float y)
{
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}
void Shader::setVector2f(const char* name, const glm::vec2& value)
{
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void Shader::setVector3f(const char* name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void Shader::setVector3f(const char* name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const char* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
void Shader::setVector4f(const char* name, const glm::vec4& value)
{
	glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4f(const char* name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- \n";
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- \n";
		}
	}
}