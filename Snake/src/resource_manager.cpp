#include "resource_manager.h"

// Instantiate static variables
std::map<const char*, Shader>       ResourceManager::shaders;
std::map<const char*, Texture2D>    ResourceManager::textures;

Shader ResourceManager::getShader(const char* name)
{
	return shaders[name];
}

Texture2D ResourceManager::getTexture(const char* name)
{
	return textures[name];
}

void ResourceManager::clearMem()
{
	// Delete all Shaders Used
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);
	// Deletes all Textures Used
	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}