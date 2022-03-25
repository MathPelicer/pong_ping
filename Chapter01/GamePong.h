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


// Game class
class GamePong
{
public:
	GamePong();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void RunPongSingleplayer(float deltaTime);
	void RunPongMultiplayer(float deltaTime);
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
	// Player count
	int playerCount;

	// Pong specific
	// Player 1
	PlayerPaddle player_1;

	// Player 2
	PlayerPaddle player_2;

	// Players array
	PlayerPaddle player_array[2] = {player_1, player_2};

	// Player Health (singleplayer)
	int health;
	// Player Score (singleplayer)
	int singleplayer_score;
	// Times the ball was rebounced (multiplayer)
	int ball_rebounce;
	// Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;

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
	SpecialBallVector2 Rainball;

	// Balls array
	BallVector2 ball_array[10] = {Ball0, Ball1, Ball2, Ball3, Ball4, Ball5, Ball6, Ball7, Ball8, Ball9};
};
