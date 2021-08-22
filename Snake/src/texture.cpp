#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D()
	: width(1), height(1), internalFormat(GL_RGBA), imageFormat(GL_RGBA), wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_NEAREST), filterMag(GL_LINEAR), tilingX(1), tilingY(1)
{
	glGenTextures(1, &this->ID);
}

void Texture2D::loadTextureFromFile(const char* filePath, bool genMipMaps)
{
	// Flip image vertically. so the origin starts from bottom-left.
	stbi_set_flip_vertically_on_load(true);

	// Load image data.
	int _width, _height, _num_of_channels;
	GLubyte* data = stbi_load(filePath, &_width, &_height, &_num_of_channels, STBI_rgb_alpha);

	this->width = _width;
	this->height = _height;

	// Bind texture.
	glBindTexture(GL_TEXTURE_2D, this->ID);

	// Set texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);

	// Generate texture from loaded data.
	glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->width, this->height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);

	// Generate mipmaps.
	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind texture.
	glBindTexture(GL_TEXTURE_2D, 0);

	// Free image data.
	stbi_image_free(data);
}

void Texture2D::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}