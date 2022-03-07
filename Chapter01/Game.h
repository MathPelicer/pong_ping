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

struct BallVector2
{

	float pos_x;
	float pos_y;

	float speed_x;
	float speed_y;

	bool active = false;

	int requirement;

	int value;

	int red;
	int blue;
	int green;

};

struct SpecialBallVector2
{

	float pos_x;
	float pos_y;

	float speed_x;
	float speed_y;

	bool active = false;

	int cooldown = 15;

	int value;

	int red[15] = { 255 , 255 , 255 , 255 , 220 , 140 , 0 , 0 , 0 , 0 , 0 , 100, 175, 255, 255};
	int blue[15] = { 0 , 85 , 140 , 220 , 255 , 255 , 255 , 255 , 255 , 150 , 0 , 0 , 0 , 0 , 0};
	int green[15] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 100 , 210 , 255 , 255 , 255, 255, 255 , 150};

};


// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
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
	
	// Pong specific
	// Direction of paddle
	int mPaddleDir;
	// Position of paddle
	Vector2 mPaddlePos;

	// Player Score
	int score;
	// Player Health
	int health;
	// Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;

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
	//SpecialBallVector2 Rainball

	BallVector2 ball_array[10] = {Ball0, Ball1, Ball2, Ball3, Ball4, Ball5, Ball6, Ball7, Ball8, Ball9};
};
