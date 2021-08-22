#pragma once

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include "renderer.h"

struct Cell
{
	// Position coords of cell
	int x;
	int y;

	// Overloaded operators for cell struct
	inline bool operator==(const Cell& rhs) const
	{
		return (this->x == rhs.x) && (this->y == rhs.y);
	}
	inline bool operator!=(const Cell& rhs) const
	{
		return !(*this == rhs);
	}
};

class GameObject
{
public:
	Cell cell;
	// Object attributes
	glm::vec2 position;
	glm::vec2 scale;
	glm::vec4 color;

	Texture2D texture;
	// Constructor
	GameObject(Cell oCell, glm::vec2 oScale, glm::vec4 oColor, Texture2D oTexture);

	void drawObject(Renderer& renderer);
};