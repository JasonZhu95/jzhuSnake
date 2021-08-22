#include "game_object.h"

GameObject::GameObject(Cell oCell, glm::vec2 oScale, glm::vec4 oColor, Texture2D oTexture)
	: cell(oCell), scale(oScale), color(oColor), texture(oTexture)
{
	position.x = cell.x * scale.x - scale.x * 0.5f;
	position.y = cell.y * scale.y - scale.y * 0.5f;
}

void GameObject::drawObject(Renderer& renderer)
{
	renderer.drawQuad(this->texture, this->position, this->scale, this->color);
}