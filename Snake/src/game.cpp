#include "game.h"
#include "resource_manager.h"

#include <irrklang/irrKlang.h>
using namespace irrklang;

#include <ctime>
#include <string>

float animationDt = 0;
ISoundEngine* SoundEngine = createIrrKlangDevice();

void Game::init(int width, int height, int tileCount)
{

	// Load Shader
	Shader quadShader;
	quadShader.loadShaderFromFile("assets/shaders/sprite.vs", "assets/shaders/sprite.fs");
	ResourceManager::shaders["quadShader"] = quadShader;

	// Load Textures
	Texture2D tileTexture;
	tileTexture.filterMin = GL_NEAREST_MIPMAP_NEAREST;
	tileTexture.loadTextureFromFile("assets/textures/tile.png", true);

	Texture2D headTexture;
	headTexture.tilingX = 2;
	headTexture.tilingY = 2;
	headTexture.filterMin = GL_LINEAR_MIPMAP_LINEAR;
	headTexture.loadTextureFromFile("assets/textures/tile.png", true);

	ResourceManager::textures["tile"] = tileTexture;
	ResourceManager::textures["head"] = headTexture;

	// Initialize Rendering
	renderer = new Renderer();
	renderer->initRenderData(width, height, quadShader);
	
	// Set values
	this->width = width;
	this->height = height;
	this->resolution = static_cast<float>(height) / width;
	this->tileX = tileCount;
	this->tileY = this->tileX * resolution;
	cellWidth = (static_cast<float>(width) / this->tileX);
	cellHeight = (static_cast<float>(height) / this->tileY);
	currentDirection = Direction::UP;
	oldDirection = currentDirection;

	// Initialize Snake head
	snakeCurrentCell = { 8, 3 };
	snakeLastCell = snakeCurrentCell;

	// Initialize Grid of Background Tiles
	for (int y = 1; y <= this->tileY; y++)
	{
		for (int x = 1; x <= this->tileX; x++)
		{
			Cell cellUnit = { x, y };
			if (cellUnit != snakeCurrentCell)
				emptyCells.push_back(cellUnit);
		}
	}

	// Configure Snake head
	GameObject snakeHead = GameObject(snakeCurrentCell, { cellWidth * 1.05f, cellHeight * 1.1f }, { 0.0f, 1.0f, 0.2f, 1.0f }, headTexture);
	snakeHead.position = glm::vec2(snakeCurrentCell.x * cellWidth - cellWidth * 0.5f, snakeCurrentCell.y * cellHeight - cellHeight * 0.5f);
	snake.push_back(snakeHead);

	// Configure Snake Tail
	Cell snakeBodyCell = { 8, 2 };
	GameObject snakeBody = GameObject(snakeBodyCell, snake[0].scale * 0.85f, { 0.0f, 1.0f, 0.2f, 1.0f }, tileTexture);
	snakeBody.position = glm::vec2(snakeBodyCell.x * cellWidth - cellWidth * 0.5f, snakeBodyCell.y * cellHeight - cellHeight * 0.5f);
	snake.push_back(snakeBody);

	// Randomize Seed
	srand(static_cast<unsigned int>(time(0)));

	// Configure Food
	food = new GameObject({ 2, 2 }, { cellWidth * 0.7f, cellHeight * 0.7f }, { 1.0f, 0.0f, 0.0f, 1.0f }, tileTexture);
	food->cell = emptyCells[rand() % emptyCells.size()];
	food->position = glm::vec2(food->cell.x * cellWidth - cellWidth * 0.5f, food->cell.y * cellHeight - cellHeight * 0.5f);

	// Initialize Member variables;
	gamePaused = false;
	gameOver = false;

	spawnObstacle();

	SoundEngine->play2D("assets/audio/music.wav", true);
}

void Game::processInput()
{
	// Directional Input
	if (!gamePaused)
	{
		if (Keys[KEY_W] || Keys[KEY_UP])
		{
			if (oldDirection == Direction::LEFT || oldDirection == Direction::RIGHT)
				currentDirection = Direction::UP;
		}
		else if (Keys[KEY_S] || Keys[KEY_DOWN])
		{
			if (oldDirection == Direction::LEFT || oldDirection == Direction::RIGHT)
				currentDirection = Direction::DOWN;
		}
		else if (Keys[KEY_A] || Keys[KEY_LEFT])
		{
			if (oldDirection == Direction::UP || oldDirection == Direction::DOWN)
				currentDirection = Direction::LEFT;
		}
		else if (Keys[KEY_D] || Keys[KEY_RIGHT])
		{
			if (oldDirection == Direction::UP || oldDirection == Direction::DOWN)
				currentDirection = Direction::RIGHT;
		}
	}

	// Restart Game
	if (Keys[KEY_R])
		restart();

	// Pause Game
	if (Keys[KEY_P])
	{
		SoundEngine->play2D("assets/audio/pause.mp3", false);
		if (!gameOver)
			gamePaused = !gamePaused;
	}
}

void Game::update(float dt)
{
	// Text Animation
	static float tempdt = dt;		//2x faster than current delta time for the animation
	tempdt += dt * 2.5;
	animationDt = abs(sin(tempdt)) - 0.5f;

	if (!gamePaused && !gameOver)
	{
		stepTimer += dt * snakeSpeed;
		if (stepTimer >= 1.0f)
		{
			stepTimer = 0;
			snakeLastCell = snakeCurrentCell;
			switch (currentDirection)
			{
				case Direction::UP:
				{
					snakeCurrentCell.y += 1;
				} break;
				case Direction::DOWN:
				{
					snakeCurrentCell.y -= 1;
				} break;
				case Direction::LEFT:
				{
					snakeCurrentCell.x -= 1;
				} break;
				case Direction::RIGHT:
				{
					snakeCurrentCell.x += 1;
				} break;
			}
			// Clear last cell that snake was in
			Cell lastTailCell = snake[snake.size() - 1].cell;
			emptyCells.push_back(lastTailCell);

			// Move snake
			for (int i = snake.size() - 1; i > 0; i--)
			{
				snake[i].cell = snake[i - 1].cell;
				snake[i].position = snake[i - 1].position;
			}

			// Move snake head
			if (snakeCurrentCell.x <= 0)
				snakeCurrentCell.x = tileX;
			else if (snakeCurrentCell.x > tileX)
				snakeCurrentCell.x = 1;

			if (snakeCurrentCell.y <= 0)
				snakeCurrentCell.y = tileY;
			else if (snakeCurrentCell.y > tileY)
				snakeCurrentCell.y = 1;

			snake[0].cell = snakeCurrentCell;
			snake[0].position = glm::vec2(snakeCurrentCell.x * cellWidth - cellWidth * 0.5f, snakeCurrentCell.y * cellHeight - cellHeight * 0.5f);

			removeCells(snake[0].cell);

			oldDirection = currentDirection;
		}
		// Collision check with tail
		for (int i = 1; i < snake.size(); i++)
		{
			if (snakeCurrentCell == snake[i].cell)
			{
				SoundEngine->play2D("assets/audio/crash.wav", false);
				snakeCurrentCell = snakeLastCell;
				snakeSpeed = 0;
				gameOver = true;
			}
		}
		// Collision Check with obstacle
		for (int i = 0; i < obstacles.size(); i++)
		{
			if (snake[0].cell == obstacles[i].cell)
			{
				SoundEngine->play2D("assets/audio/crash.wav", false);
				snakeCurrentCell = snakeLastCell;
				snakeSpeed = 0;
				gameOver = true;
			}
		}
		// Collision with Food
		if (snake[0].cell == food->cell)
		{

			SoundEngine->play2D("assets/audio/eat.wav", false);
			// Increase Score
			score += 10;

			// Increase Speed
			snakeSpeed += 0.3f;

			// Spawn obstacle
			if (rand() % 100 + 1 > 50)
				spawnObstacle();

			// Grow Tail
			Cell offset = snakeLastCell;
			if (snake.size() > 1)
			{
				// Check end of snake cell position and add new tail
				Cell lastCell = snake[snake.size() - 1].cell;
				Cell secondLastCell = snake[snake.size() - 2].cell;
				offset = { 2 * lastCell.x - secondLastCell.x, 2 * lastCell.y - secondLastCell.y };
				// Check if within map
				if (offset.x <= 0)
					offset.x = tileX - lastCell.x;
				else if (offset.x > tileX)
					offset.x = lastCell.x + 1;
				if (offset.y <= 0)
					offset.y = tileY - lastCell.y;
				else if (offset.y > tileY)
					offset.y = lastCell.y + 1;
			}
			// Add tail object and texture to end of snake
			Texture2D snakeBodyTexture = ResourceManager::getTexture("tile");
			GameObject tail = GameObject(offset, snake[0].scale * 0.85f, glm::vec4(1.0f), snakeBodyTexture);
			tail.position = glm::vec2(tail.cell.x * cellWidth - cellWidth * 0.5f, tail.cell.y * cellHeight - cellHeight * 0.5f);
			snake.push_back(tail);
			removeCells(tail.cell);

			// Make new food object
			food->cell = emptyCells[rand() % emptyCells.size()];
			food->position = glm::vec2(food->cell.x * cellWidth - cellWidth * 0.5f, food->cell.y * cellHeight - cellHeight * 0.5f);
		}
	}

	// Win the Game if no more empty spaces
	if (emptyCells.size() < 1)
		gamePaused = true;

	// Clear key each frame
	for (auto& i : supportedKeys)
		Keys[i] = false;
}

void Game::render()
{
	//Clear screen
	renderer->clear(glm::vec4(0.0f));

	// Draw grid tiles
	Texture2D gridTexture = ResourceManager::getTexture("tile");
	gridTexture.tilingX = tileX;
	gridTexture.tilingY = tileY;
	renderer->drawQuad(gridTexture, { this->width * 0.5f, this->height * 0.5f }, { this->width, this->height }, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));

	// Draw snake and head
	for (int i = snake.size() - 1; i >= 0; i--)
	{
		float percent = static_cast<float>(i) / static_cast<float>(snake.size());
		glm::vec3 bodyColor = glm::mix(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), percent);
		snake[i].color = glm::vec4(bodyColor.r, bodyColor.g, bodyColor.b, 1.0f);
		snake[i].drawObject(*renderer);
	}

	// Draw Food
	food->drawObject(*renderer);

	// Draw obstacle
	for (unsigned int i = 0; i < obstacles.size(); i++)
		obstacles[i].drawObject(*renderer);

	// Draw Score
	renderer->drawText(std::to_string(score), {this->width * 0.5f, 50}, 2.5f + animationDt * .25f , glm::vec4(0.44314f, 0.9333f, 0.72157f, 1.0f));

	// Draw Pause
	if (gamePaused)
	{
		renderer->drawText("PAUSED", { this->width * 0.5f, this->height * 0.5f - 40 }, 5.0f + animationDt * .25f, glm::vec4(0.996f, 0.635f, 0.003f, 1.0f));
		renderer->drawText("Press [P] to unpause.", { this->width * 0.5f, this->height * 0.5f + 30 }, 2.0f + animationDt * .25f, glm::vec4(1.0, 0.8f, 0.2f, 1.0f));
	}

	// Draw Game Over
	if (gameOver)
	{
		renderer->drawText("GAME OVER", { this->width * 0.5f, this->height * 0.5f - 40 }, 5.0f + animationDt * .25f, glm::vec4(1.0, 0.1f, 0.1f, 1.0f));
		renderer->drawText("Press [R] to restart.", { this->width * 0.5f, this->height * 0.5f + 30 }, 3.0f + animationDt * .25f, glm::vec4(0.5, 0.8f, 0.2f, 1.0f));
	}
}

void Game::removeCells(Cell cell)
{
	auto it = std::find(emptyCells.begin(), emptyCells.end(), cell);
	if (it != emptyCells.end())
	{
		//Swap element to the back and pop it off
		std::swap(*it, emptyCells.back());
		emptyCells.pop_back();
	}
}

void Game::spawnObstacle()
{
	// Create texture and randomize tile position
	Texture2D obstacleTexture = ResourceManager::getTexture("tile");
	obstacleTexture.tilingX = 1 + (rand() % 2);
	obstacleTexture.tilingY = 1 + (rand() % 2);

	// Grab a random empty cell
	Cell eCell = emptyCells[rand() % emptyCells.size()];

	// Create an obstacle
	GameObject obstacle = GameObject(eCell, { cellWidth * 0.7f, cellHeight * 0.7f }, { 0.6f, 0.6f, 0.6f, 1.0f }, obstacleTexture);
	obstacle.position = glm::vec2(eCell.x * cellWidth - cellWidth * 0.5f, eCell.y * cellHeight - cellHeight * 0.5f);

	// Add obstacle to list
	obstacles.push_back(obstacle);

	// Remove the cell from list of available cells
	removeCells(eCell);
}

// TODO: Redundant code will fix after
void Game::restart()
{
	// Clear data
	snake.clear();
	emptyCells.clear();
	obstacles.clear();

	currentDirection = Direction::UP;
	oldDirection = currentDirection;

	// Initialize Snake head
	snakeCurrentCell = { 8, 3 };
	snakeLastCell = snakeCurrentCell;

	// Initialize Grid of Background Tiles
	for (int y = 1; y <= this->tileY; y++)
	{
		for (int x = 1; x <= this->tileX; x++)
		{
			Cell cellUnit = { x, y };
			if (cellUnit != snakeCurrentCell)
				emptyCells.push_back(cellUnit);
		}
	}

	// Configure Snake head
	Texture2D headTexture = ResourceManager::getTexture("head");
	GameObject snakeHead = GameObject(snakeCurrentCell, { cellWidth * 1.05f, cellHeight * 1.1f }, { 0.0f, 1.0f, 0.2f, 1.0f }, headTexture);
	snakeHead.position = glm::vec2(snakeCurrentCell.x * cellWidth - cellWidth * 0.5f, snakeCurrentCell.y * cellHeight - cellHeight * 0.5f);
	snake.push_back(snakeHead);

	// Configure Snake Tail
	Texture2D tileTexture = ResourceManager::getTexture("tile");
	Cell snakeBodyCell = { 8, 2 };
	GameObject snakeBody = GameObject(snakeBodyCell, snake[0].scale * 0.85f, { 0.0f, 1.0f, 0.2f, 1.0f }, tileTexture);
	snakeBody.position = glm::vec2(snakeBodyCell.x * cellWidth - cellWidth * 0.5f, snakeBodyCell.y * cellHeight - cellHeight * 0.5f);
	snake.push_back(snakeBody);

	// Randomize Seed
	srand(static_cast<unsigned int>(time(0)));

	// Spawn obstacle
	spawnObstacle();

	// Configure Food
	food->cell = emptyCells[rand() % emptyCells.size()];
	food->position = glm::vec2(food->cell.x * cellWidth - cellWidth * 0.5f, food->cell.y * cellHeight - cellHeight * 0.5f);

	// Initialize Member variables;
	gamePaused = false;
	gameOver = false;

	score = 0;
	snakeSpeed = 8.0f;
}

void Game::updateDimensions(int width, int height)
{
	// Checks if the game window is minimized and if so, pause the game.
	if (width == 0 || height == 0)
	{
		if (!gameOver)
		{
			gamePaused = true;
			return;
		}
	}

	this->width = width;
	this->height = height;
	this->resolution = static_cast<float>(height) / static_cast<float>(width);

	cellWidth = (static_cast<float>(width) / tileX);
	cellHeight = (static_cast<float>(height) / tileY);

	// Rescale snake.
	for (unsigned int i = 0; i < snake.size(); ++i)
	{
		GameObject* snake_part = &snake[i];

		if (i == 0)
			snake_part->scale = { cellWidth * 1.1f, cellHeight * 1.1f };
		else
			snake_part->scale = { cellWidth * 0.9f, cellHeight * 0.9f };

		snake_part->position = glm::vec2(snake_part->cell.x * cellWidth - cellWidth * 0.5f, snake_part->cell.y * cellHeight - cellHeight * 0.5f);
	}

	// Scale apple.
	food->scale = { cellWidth * 0.7f, cellHeight * 0.7f };
	food->position = glm::vec2(food->cell.x * cellWidth - cellWidth * 0.5f, food->cell.y * cellHeight - cellHeight * 0.5f);

	// Scale Obstacle
	for (unsigned int i = 0; i < obstacles.size(); ++i)
	{
		GameObject* obstacle = &obstacles[i];
		obstacle->scale = { cellWidth * 0.7f, cellHeight * 0.7f };
		obstacle->position = glm::vec2(obstacle->cell.x * cellWidth - cellWidth * 0.5f, obstacle->cell.y * cellHeight - cellHeight * 0.5f);
	}

	renderer->renderDimensions(width, height);
}

void Game::endGame()
{
	ResourceManager::clearMem();

	delete food;
	delete renderer;
}