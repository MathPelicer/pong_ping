#pragma once

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

	bool draw_only;

	bool active;

	int requirement;

	int value;

	int red;
	int green;
	int blue;

};

struct SpecialBallVector2
{

	Vector2 mBallPos;

	Vector2 mBallSpeed;

	bool active = false;

	int cooldown = 250;

	int colour_changer_timer = 15;

	int colour_changer = 0;

	int red = 255;
	int green = 0;
	int blue = 0;

};