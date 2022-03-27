// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include "Game.h"

struct BlockVector2
{
	Vector2 mBlockPos;

	int health;

	int value;
};


// Game class
class GameArkanoid
{
public:
	GameArkanoid();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void RunArkanoid(float deltaTime);
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;

	// Arkanoid specific
	// Player
	PlayerPaddle player_1;
	int health;

	// Shoot ball (Start game)
	bool ball_shot;
	int ball_direction;

	// On the Ball - Bryce Vine
	BallVector2 Ball0;
	BallVector2 Ball1;
	BallVector2 Ball2;
	BallVector2 Ball3;
	BallVector2 Ball4;
	BallVector2 Ball5;
	BallVector2 Ball6;
	BallVector2 Ball7;
	BallVector2 Ball8;
	BallVector2 Ball9;

	// Balls array
	BallVector2 ball_array[10] = { Ball0, Ball1, Ball2, Ball3, Ball4, Ball5, Ball6, Ball7, Ball8, Ball9 };

	// Block

	BlockVector2 Block0;
	BlockVector2 Block1;
	BlockVector2 Block2;
	BlockVector2 Block3;
	BlockVector2 Block4;

	// Blocks array

	BlockVector2 block_array[5] = { Block0, Block1, Block2, Block3, Block4 };

};
