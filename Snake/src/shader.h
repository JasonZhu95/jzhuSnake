#pragma once
#include <string>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

class Shader
{
public:
	// State
	unsigned int ID;
	// Constructor
	Shader() { }
	// Activate Shader passed by reference
	void	use();
	// Compile the shader, geometry shader optional
	void    compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	// Load Shader from File
	void	loadShaderFromFile(const char* vShaderPath, const char* fShaderPath);
	// OpenGl utility function to set uniforms
	void    setFloat(const char* name, float value);
	void    setInteger(const char* name, int valuee);
	void    setVector2f(const char* name, float x, float y);
	void    setVector2f(const char* name, const glm::vec2& value);
	void    setVector3f(const char* name, float x, float y, float z);
	void    setVector3f(const char* name, const glm::vec3& value);
	void    setVector4f(const char* name, float x, float y, float z, float w);
	void    setVector4f(const char* name, const glm::vec4& value);
	void    setMatrix4f(const char* name, const glm::mat4& matrix);
private:
	// Debug to check for any errors.
	void		checkCompileErrors(unsigned int object, std::string type);
	// Load shader source
	std::string loadShaderSourceFromFile(std::string path);
};