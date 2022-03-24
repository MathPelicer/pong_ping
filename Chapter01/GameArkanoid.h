// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};

struct PlayerPaddle
{
	// Direction of paddle
	int mPaddleDir;

	// Reverse paddle
	bool mPaddleReverse;
	int mPaddleReverseWait;

	// Position of paddle
	Vector2 mPaddlePos;

	// Player Score
	int score;
};

struct BallVector2
{

	Vector2 mBallPos;

	Vector2 mBallSpeed;

	bool active;

	int requirement;

	int value;

	int red;
	int green;
	int blue;

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

	// Player Health
	int health;

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
};
