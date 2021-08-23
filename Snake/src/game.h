#pragma once

#include "renderer.h"
#include "game_object.h"

#include <vector>

#include <GLFW/glfw3.h>

// Direction the snake is moving
enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};
// Key Options to move, pause and restart
enum KeyOptions
{
	KEY_W             = GLFW_KEY_W, 
	KEY_S             = GLFW_KEY_S, 
	KEY_A             = GLFW_KEY_A, 
	KEY_D             = GLFW_KEY_D, 
	KEY_P             = GLFW_KEY_P, 
	KEY_R             = GLFW_KEY_R, 
	KEY_UP            = GLFW_KEY_UP,
	KEY_DOWN          = GLFW_KEY_DOWN,
	KEY_LEFT          = GLFW_KEY_LEFT,
	KEY_RIGHT         = GLFW_KEY_RIGHT,
};

class Game
{
public:
	// Game State
	bool Keys[1024];				//Check key input
	// Initialize Game state
	void init(int width, int height, int tileCount);
	// Game Loop
	void processInput();			// Store user input
	void update(float dt);			// Update Game Events
	void render();					// Render the Game
	void removeCells(Cell cell);	// Remove empty cell
	void spawnObstacle();			// Spawn an obstacle
	void restart();					// Restart the game on r press
	void updateDimensions(int width, int height);	// Update game dimensions 
	void endGame();					// Clear memory
private:
	// Check Snake position
	Cell      snakeLastCell;
	Cell      snakeCurrentCell;

	// Check Snake Direction
	Direction oldDirection;
	Direction currentDirection;

	// Create renderer and food object
	Renderer* renderer;
	GameObject* food;
	// Vectors to add snake obstacles and empty cells
	std::vector<Cell>       emptyCells;
	std::vector<GameObject> snake;
	std::vector<GameObject> obstacles;

	// Check keys processed
	int keysProcessed[10] = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_S, KEY_A, KEY_D, KEY_P, KEY_R };

	// Positional variables
	int width;
	int height;
	int tileX;
	int tileY;

	// Resolution variables
	float resolution;
	float cellWidth;
	float cellHeight;

	// Game update score, timer, and speed of snake
	unsigned int score = 0;
	float stepTimer = 0;
	float snakeSpeed = 8.0f;

	// Check for gaame over or pause
	bool gamePaused;
	bool gameOver;

};