#pragma once
#include <map>

#include "texture.h"
#include "shader.h"

class ResourceManager
{
public:
	// Stored Data for Shaders and Textures
	static std::map<const char*, Shader>    shaders;
	static std::map<const char*, Texture2D> textures;
	// Retrieves and activates the shader
	static Shader    getShader(const char* name);
	// Retrieves and activates the texture
	static Texture2D getTexture(const char* name);
	// Deallocate memory
	static void      clearMem();
private:
	// Private 
	ResourceManager() { }
};