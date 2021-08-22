#include "renderer.h"

// Needed for GLText
#define GLT_IMPLEMENTATION
#define GLT_MANUAL_VIEWPORT
#include "gltext.h"

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &this->quadVAO);
	gltTerminate();
}

void Renderer::initRenderData(float width, float height, Shader& quadShader)
{
	// configure VAO/VBO
	unsigned int VBO;
	float vertices[] = {
		// Position		// TexCoords
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,

		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,	0.0f, 1.0f
	};

	// Generate VAO and VBO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &VBO);

	// Bind Data for VAO/ VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(quadVAO);

	// Vertex Attributes
	// Vertex Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	// Vertex Texture Coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// OpenGL config.
	glEnable(GL_CULL_FACE);

	// Blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth mask.
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	//Text Rendering
	gltInit();

	this->quadShader = quadShader;
	this->renderWidth = width;
	this->renderHeight = height;

	renderDimensions(width, height);
}

void Renderer::clear(glm::vec4 color)
{
	glClearDepth(1.0f);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawQuad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, glm::vec4 color)
{
	// OpenGL Blend Configuration
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Configure projection and model matrices
	glm::mat4 projection = glm::ortho<float>(0.0f, this->renderWidth, 0.0f, this->renderHeight, -1.0f, 1.0f);
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

	this->quadShader.use();
	this->quadShader.setMatrix4f("projection", projection);
	this->quadShader.setMatrix4f("model", model);
	this->quadShader.setVector2f("tiling", texture.tilingX, texture.tilingY);
	this->quadShader.setVector4f("color", color.x, color.y, color.z, color.w);
	// Bind Texture
	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	// Render quad
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::drawQuad(Texture2D& texture, glm::vec2 pos, glm::vec2 scale, float rotate, glm::vec4 color)
{
	// OpenGL Blend Configuration
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Configure projection and model matrices
	glm::mat4 projection = glm::ortho<float>(0.0f, this->renderWidth, 0.0f, this->renderHeight, -1.0f, 1.0f);
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

	this->quadShader.use();
	this->quadShader.setMatrix4f("projection", projection);
	this->quadShader.setMatrix4f("model", model);
	this->quadShader.setVector2f("tiling", texture.tilingX, texture.tilingY);
	this->quadShader.setVector4f("color", color.x, color.y, color.z, color.w);
	// Bind Texture
	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	// Render Quad with rotate
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::drawText(std::string text_to_draw, glm::vec2 position, float size, glm::vec4 color)
{
	// Create Text
	GLTtext* text = gltCreateText();
	gltSetText(text, text_to_draw.c_str());

	// Draw Text
	gltBeginDraw();

	// Color and Align the text
	gltColor(color.x, color.y, color.z, color.w);
	gltDrawText2DAligned(text, position.x, position.y, size, GLT_CENTER, GLT_CENTER);

	// Finish and clear memory
	gltEndDraw();
	gltDeleteText(text);
}

void Renderer::renderDimensions(int width, int height)
{
	// Reset viewport to given dimensions
	glViewport(0, 0, width, height);

	// Adjust text on resize
	if (width > 0 && height > 0)
	{
		gltViewport(width, height);
	}
	this->renderWidth = width;
	this->renderHeight = height;
	// Configure the projection matrix for adjusted window
	glm::mat4 projection = glm::ortho<float>(0.0f, width, 0.0f, height, -1.0f, 1.0f);
	this->quadShader.use();
	this->quadShader.setMatrix4f("projection", projection);
}