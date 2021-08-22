#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
	// Holds ID for texture to be used
	unsigned int ID;
	// Size of the Texture to be loaded
	unsigned int width, height;
	// Format of the Texture
	unsigned int internalFormat;   // Texture Object
	unsigned int imageFormat;		// Loaded Image
	// Configure Texture Type Padding
	unsigned int wrapS;
	unsigned int wrapT;
	unsigned int filterMin;
	unsigned int filterMag;
	// Grid Tiling
	unsigned int tilingX;
	unsigned int tilingY;
	// Constructor
	Texture2D();
	// Uses Image Data to Create a Texture
	void loadTextureFromFile(const char* filePath, bool genMipMaps = false);
	// Binds the Texture to most recent for OpenGL
	void bind() const;
};