#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"


class Renderer
{
public:
	// Destructor
	~Renderer();
	// Initializes and configures the quad's buffer and vertex attributes
	void initRenderData(float width, float height, Shader& quadShader);
	// Clear and set up OpenGl Config
	void clear(glm::vec4 color);
	// Renders a defined quad textured with given sprite
	void drawQuad(Texture2D& texture, glm::vec2 position, glm::vec2 size, glm::vec4 color);
	// Overload with rotation
	void drawQuad(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec4 color);
	// Render text
	void drawText(std::string text_to_draw, glm::vec2 position, float size, glm::vec4 color);
	// Update Dimensions if window is resized
	void renderDimensions(int width, int height);

private:

	int renderWidth;
	int renderHeight;

	// Render state
	Shader       quadShader;
	unsigned int quadVAO;
};