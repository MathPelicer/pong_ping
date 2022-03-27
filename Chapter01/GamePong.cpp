// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "GamePong.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int thickness = 15;//sera usado para setar a altura de alguns objetos
const float paddleH = 100.0f;//tamanho da raquete

GamePong::GamePong()
:mWindow(nullptr)//para criar uma janela
,mRenderer(nullptr)//para fins de renderiza��o na tela
,mTicksCount(0)//para guardar o tempo decorrido no jogo
,mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
{

}

bool GamePong::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	srand(time(NULL));

	player_array[0].mPaddlePos.x = 10.0f;//posi��o inicial da raquete eixo x
	player_array[0].mPaddlePos.y = 768.0f/2.0f;//posi��o inicial da raquee eixo y
	player_array[0].mPaddleReverseWait = 0;
	player_array[0].mPaddleDir = 0;
	player_array[0].score = 0;
	
	// Inicializa o player 2
	player_array[1].mPaddlePos.x = 980.0f;//posi��o inicial da raquete eixo x
	player_array[1].mPaddlePos.y = 768.0f / 2.0f;//posi��o inicial da raquee eixo y
	player_array[1].mPaddleReverseWait = 0;
	player_array[1].mPaddleDir = 0;
	player_array[1].score = 0;

	// Define a quantidade de jogadores
	playerCount = 1;
	
	// Define a vida do jogador
	health = 3;

	// Define a pontuação inicial do jogador (singleplayer)
	singleplayer_score = 0;

	// Define a pontuação total dos jogadores (multiplayer)
	ball_rebounce = 0;

	// Inicializa as bolas

	for (int i = 0; i < 10; i++) {
		// Define a velocidade inicial da bola
		ball_array[i].mBallSpeed.x = ((rand() % 100) + ((i + 1) * 100)) * ( - 1);
		ball_array[i].mBallSpeed.y = rand() % 500;
		//Define a pontua��o da bola
		ball_array[i].value = i + 1;
		//Define a pontua��o necess�ria para ativar a bola
		ball_array[i].requirement = (i * 5) * 1.4;
		//Define as cores da bola
		ball_array[i].red = rand() % 200 + 55;
		ball_array[i].blue = rand() % 200 + 55;
		ball_array[i].green = rand() % 200 + 55;
	}

	return true;
}

void GamePong::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void GamePong::ProcessInput()
{
	SDL_Event event;//evento, inputs do jogador s�o armazenados aqui
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard - podemos buscar por alguma tecla espec�fica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys - atualize a dire��o da raquete com base na entrada do jogador
	// W -> move a raquete para cima, S -> move a raquete para baixo
	
	// Reseta as variáveis do player 1
	player_array[0].mPaddleDir = 0;
	player_array[0].mPaddleReverse = false;

	// Reseta as variáveis do player 2
	player_array[1].mPaddleDir = 0;
	player_array[1].mPaddleReverse = false;

	// Verifica qual o input do teclado

	// Input para "subir" o player 1
	if (state[SDL_SCANCODE_W])
	{
		player_array[0].mPaddleDir -= 1;
	}
	// Input para "descer" o player 1
	if (state[SDL_SCANCODE_S])
	{
		player_array[0].mPaddleDir += 1;
	}
	// Input para reverter a raquete do player 1
	if (state[SDL_SCANCODE_SPACE] && player_1.mPaddleReverseWait == 0)
	{
		player_array[0].mPaddleReverse = true;
	}

	// Inputs player 2

	// Input para iniciar o player 2
	if (state[SDL_SCANCODE_2])
	{
		playerCount = 2;
	}

	// Input para "subir" o player 2
	if (state[SDL_SCANCODE_UP])
	{
		player_array[1].mPaddleDir -= 1;
	}
	// Input para "descer" o player 2
	if (state[SDL_SCANCODE_DOWN])
	{
		player_array[1].mPaddleDir += 1;
	}
	// Input para reverter a raquete do player 2
	if (state[SDL_SCANCODE_RETURN] && player_array[1].mPaddleReverseWait == 0)
	{
		player_array[1].mPaddleReverse = true;
	}
	// Fecha o jogo
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
}

void GamePong::RunPongSingleplayer(float deltaTime)
{
	// atualiza a posi��o da raquete
	if (player_array[0].mPaddleDir != 0)
	{
		player_array[0].mPaddlePos.y += player_array[0].mPaddleDir * 300.0f * deltaTime;
		// verifique que a raquete n�o se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (player_array[0].mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			player_array[0].mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (player_array[0].mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			player_array[0].mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	

	if (player_array[0].mPaddleReverse)
	{
		player_array[0].mPaddlePos.y = 768 - player_array[0].mPaddlePos.y;
		player_array[0].mPaddleReverseWait = 50;
	}

	if (player_array[0].mPaddleReverseWait > 0) {
		player_array[0].mPaddleReverseWait--;
	}
	

	/*========================================*/
	/*             Nosso C�digo               */
	/*========================================*/

	//Tenta invocar a bola especial

	if (!Rainball.active) {

		int nat20 = rand() % 20 + 1;

		if (nat20 == 20) {
			Rainball.cooldown--;
			if (Rainball.cooldown == 0) {
				Rainball.active = true;
				Rainball.mBallPos.x = rand() % 256 + 768;
				Rainball.mBallPos.y = rand() % 768;
				Rainball.mBallSpeed.x = ((rand() % 250) + 75) * (-1);
				Rainball.mBallSpeed.y = rand() % 300;

			}
		}

	}

	//Timer para mudar a cor da bola especial

	int i;

	if (Rainball.active) {
		Rainball.colour_changer_timer--;
		//Temporizador para trocar a cor chega a 0
		if (Rainball.colour_changer_timer == 0) {
			//Troca a cor da bola
			for (i = 0; i < 10; i++) {
				if (Rainball.red == 255 && (Rainball.green >= 0 && Rainball.green < 255) && Rainball.blue == 0) {
					Rainball.green++;
				}
				else if ((Rainball.red > 0 && Rainball.red <= 255) && Rainball.green == 255 && Rainball.blue == 0) {
					Rainball.red--;
				}
				else if (Rainball.red == 0 && Rainball.green == 255 && (Rainball.blue >= 0 && Rainball.blue < 255)) {
					Rainball.blue++;
				}
				else if (Rainball.red == 0 && (Rainball.green > 0 && Rainball.green <= 255) && Rainball.blue == 255) {
					Rainball.green--;
				}
				else if ((Rainball.red >= 0 && Rainball.red < 255) && Rainball.green == 0 && Rainball.blue == 255) {
					Rainball.red++;
				}
				else if (Rainball.red == 255 && Rainball.green == 0 && (Rainball.blue > 0 && Rainball.blue <= 255)) {
					Rainball.blue--;
				}
				//Reseta o temporizador
				Rainball.colour_changer_timer = 15;
			}
		}
	}

	//Verifica colisão das bolas

	for (i = 0; i < 10; i++) {

		if (ball_array[i].requirement <= singleplayer_score && !ball_array[i].active) {
			// Define a posição inicial da bola
			ball_array[i].mBallPos.x = rand() % 512 + 512;
			ball_array[i].mBallPos.y = rand() % 768;
			// Ativa a bola
			ball_array[i].active = true;
		}

		if (ball_array[i].active) {

			ball_array[i].mBallPos.x += ball_array[i].mBallSpeed.x * deltaTime;
			ball_array[i].mBallPos.y += ball_array[i].mBallSpeed.y * deltaTime;

			// atualiza a posição da bola se ela colidiu com a raquete
			float diff = player_array[0].mPaddlePos.y - ball_array[i].mBallPos.y;
			//pegue o valor absoluto de diferen�a entre o eixo y da bolinha e da raquete
			//isso � necess�rio para os casos em que no pr�ximo frame a bolinha ainda n�o esteja t�o distante da raquete
			//verifica se a bola est� na area da raquete e na mesma posi��o no eixo x
			diff = (diff > 0.0f) ? diff : -diff;
			if (
				// A diferen�a no eixo y y-difference is small enough
				diff <= paddleH / 2.0f &&
				// Estamos na posicao x correta
				ball_array[i].mBallPos.x <= 25.0f && ball_array[i].mBallPos.x >= 20.0f &&
				// A bolinha est� se movendo para a esquerda
				ball_array[i].mBallSpeed.x < 0.0f)
			{
				ball_array[i].mBallSpeed.x *= -1.0f;
			}
			//Verifica se a bola saiu da tela (no lado esquerdo, onde � permitido)
			//Se sim, encerra o jogo
			// ====================
			// FAZER ALTERAÇÃO AQUI
			// ====================
			else if (ball_array[i].mBallPos.x <= 0.0f)
			{
				if (health == 1) {
					printf("Your score was %d\n", singleplayer_score);
					mIsRunning = false;

				}
				else {
					if (health == 5) {
						health = 4;
					}
					else if (health == 4) {
						health = 3;
					}
					else if (health == 3) {
						health = 2;
					}
					else if (health == 2) {
						health = 1;
					}
					printf("Health: %d\n", health);
					ball_array[i].mBallPos.x = rand() % 256 + 768;
					ball_array[i].mBallPos.y = rand() % 768;
					ball_array[i].mBallSpeed.x *= 0.75;
					ball_array[i].mBallSpeed.y *= 0.75;
				}

			}
			// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
			// 
			else if (ball_array[i].mBallPos.x >= (1024.0f - thickness) && ball_array[i].mBallSpeed.x > 0.0f)
			{
				float speed_increment = (rand() % 21 - 10) / 100;

				ball_array[i].mBallSpeed.x *= -1.0f + speed_increment;

				printf("+%d\n", ball_array[i].value);

				singleplayer_score += ball_array[i].value;

				printf("Your score is %d\n", singleplayer_score);

			}
			// Atualize (negative) a posi��o da bola se ela colidir com a parede de cima
			// 
			if (ball_array[i].mBallPos.y <= thickness && ball_array[i].mBallSpeed.y < 0.0f)
			{
				ball_array[i].mBallSpeed.y *= -1;
			}
			// Atualize (negative) a posi��o da bola se ela colidiu com a parede de baixo
			// Did the ball collide with the bottom wall?
			else if (ball_array[i].mBallPos.y >= (768 - thickness) && ball_array[i].mBallSpeed.y > 0.0f)
			{
				ball_array[i].mBallSpeed.y *= -1;
			}

		}

	}



	//=====================================
	//Verifica as colisões da bola especial
	//=====================================

	if (Rainball.active) {

		Rainball.mBallPos.x += Rainball.mBallSpeed.x * deltaTime;
		Rainball.mBallPos.y += Rainball.mBallSpeed.y * deltaTime;

		// atualiza a posi��o da bola se ela colidiu com a raquete
		float rainballDiff = player_array[0].mPaddlePos.y - Rainball.mBallPos.y;
		//pegue o valor absoluto de diferen�a entre o eixo y da bolinha e da raquete
		//isso � necess�rio para os casos em que no pr�ximo frame a bolinha ainda n�o esteja t�o distante da raquete
		//verifica se a bola est� na area da raquete e na mesma posi��o no eixo x
		rainballDiff = (rainballDiff > 0.0f) ? rainballDiff : -rainballDiff;
		if (
			// A diferen�a no eixo y y-difference is small enough
			rainballDiff <= paddleH / 2.0f &&
			// Estamos na posicao x correta
			Rainball.mBallPos.x <= 25.0f && Rainball.mBallPos.x >= 20.0f &&
			// A bolinha est� se movendo para a esquerda
			Rainball.mBallSpeed.x < 0.0f)
		{
			Rainball.mBallSpeed.x *= -1.0f;
		}
		//Verifica se a bola saiu da tela (no lado esquerdo, onde � permitido)
		//Se sim, encerra o jogo
		// ====================
		// FAZER ALTERA��O AQUI
		// ====================
		else if (Rainball.mBallPos.x <= 0.0f)
		{

			Rainball.mBallPos.x = -10;
			Rainball.mBallPos.y = -10;
			Rainball.mBallSpeed.x *= 0;
			Rainball.mBallSpeed.y *= 0;
			Rainball.active = false;
			Rainball.cooldown = 150;

		}
		// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
		// 
		else if (Rainball.mBallPos.x >= (1024.0f - thickness) && Rainball.mBallSpeed.x > 0.0f)
		{

			Rainball.mBallPos.x = -10;
			Rainball.mBallPos.y = -10;
			Rainball.mBallSpeed.x *= 0;
			Rainball.mBallSpeed.y *= 0;
			Rainball.active = false;
			Rainball.cooldown = 300;
			printf("Special ball scored!!!\n");

			if (health < 5) {
				printf("+1 Health\n");
				health++;
			}

		}
		// Atualize (negative) a posi��o da bola se ela colidir com a parede de cima
		// 
		if (Rainball.mBallPos.y <= thickness && Rainball.mBallSpeed.y < 0.0f)
		{
			Rainball.mBallSpeed.y *= -1;
		}
		// Atualize (negative) a posi��o da bola se ela colidiu com a parede de baixo
		// Did the ball collide with the bottom wall?
		else if (Rainball.mBallPos.y >= (768 - thickness) && Rainball.mBallSpeed.y > 0.0f)
		{
			Rainball.mBallSpeed.y *= -1;
		}

	}
}

void GamePong::RunPongMultiplayer(float deltaTime)
{

	for (int j = 0; j < 2; j++) {
		if (player_array[j].mPaddleDir != 0)
		{
			player_array[j].mPaddlePos.y += player_array[j].mPaddleDir * 300.0f * deltaTime;
			// verifique que a raquete n�o se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
			if (player_array[j].mPaddlePos.y < (paddleH / 2.0f + thickness))
			{
				player_array[j].mPaddlePos.y = paddleH / 2.0f + thickness;
			}
			else if (player_array[j].mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
			{
				player_array[j].mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}
		if (player_array[j].mPaddleReverse)
		{
			player_array[j].mPaddlePos.y = 768 - player_array[j].mPaddlePos.y;
			player_array[j].mPaddleReverseWait = 50;
		}

		if (player_array[j].mPaddleReverseWait > 0) {
			player_array[j].mPaddleReverseWait--;
		}
	}
	
	
	

	/*========================================*/
	/*             Nosso Código               */
	/*========================================*/

	//Verifica colisão das bolas
	int i;
	for (i = 0; i < 10; i++) {

		if (ball_array[i].requirement * (2 * i) <= ball_rebounce && !ball_array[i].active) {
			// Define as posições iniciais das bolas
			ball_array[i].mBallPos.x = rand() % 512;
			ball_array[i].mBallPos.y = rand() % 768;
			if (player_array[1].score > player_array[0].score) {
				ball_array[i].mBallSpeed.x *= -1;
			}
			ball_array[i].active = true;
		}

		if (ball_array[i].active) {

			ball_array[i].mBallPos.x += ball_array[i].mBallSpeed.x * deltaTime;
			ball_array[i].mBallPos.y += ball_array[i].mBallSpeed.y * deltaTime;

			// atualiza a posição da bola se ela colidiu com a raquete
			float diff1 = player_array[0].mPaddlePos.y - ball_array[i].mBallPos.y;
			float diff2 = player_array[1].mPaddlePos.y - ball_array[i].mBallPos.y;

			//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
			//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
			//verifica se a bola está na area da raquete e na mesma posição no eixo x
			diff1 = (diff1 > 0.0f) ? diff1 : -diff1;
			diff2 = (diff2 < 0.0f) ? -diff2 : diff2;
			// A diferença no eixo y y-difference é pequena o suficiente para o player 1
			if ((diff1 <= paddleH / 2.0f && 
				// Estamos na posicao x correta
				ball_array[i].mBallPos.x <= 25.0f && ball_array[i].mBallPos.x >= 20.0f && 
				// A bolinha está se movendo para a esquerda
				ball_array[i].mBallSpeed.x < 0.0f) ||
				// A diferença no eixo y y-difference é pequena o suficiente para o player 2
				(diff2 <= paddleH / 2.0f &&
				// Estamos na posicao x correta
				ball_array[i].mBallPos.x <= 985.0f && ball_array[i].mBallPos.x >= 970.0f &&
				// A bolinha está se movendo para a direita
				ball_array[i].mBallSpeed.x > 0.0f))
			{
				ball_array[i].mBallSpeed.x *= -1.03f;
				ball_rebounce++;
			}
			// A diferença no eixo y y-difference é pequena o suficiente para o player 2
			/*else if (diff2 <= paddleH / 2.0f &&
				// Estamos na posicao x correta
				ball_array[i].pos_x <= 985.0f && ball_array[i].pos_x >= 970.0f && 
				// A bolinha está se movendo para a direita
				ball_array[i].speed_x > 0.0f)
			{
				ball_array[i].speed_x *= -1.03f;
			}*/
			//Verifica se a bola saiu da tela (no lado esquerdo, onde � permitido)
			else if (ball_array[i].mBallPos.x <= 0.0f)
			{
				// Incrementa a pontuação do player 2
				player_array[1].score += ball_array[i].value;
				printf("Player 2 scored!\n");
				printf("Player 2 score = %d\n", player_array[1].score);

				//Reseta a quantidade de vezes que a bola foi rebatida
				ball_rebounce = 0;

				if (player_array[1].score >= 10) {
					printf("Player 2 won!\n");
					mIsRunning = false;
				}

				// Reposiciona a bola no jogo
				ball_array[i].mBallPos.x = rand() % 226 + 30;
				ball_array[i].mBallPos.y = rand() % 768;
				ball_array[i].mBallSpeed.x *= -0.90;
				ball_array[i].mBallSpeed.y *= 0.95;
								}
			// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
			// 
			else if (ball_array[i].mBallPos.x >= (1024.0f - thickness) && ball_array[i].mBallSpeed.x > 0.0f)
			{
				// Incrementa a pontuação do player 1
				player_array[0].score += ball_array[i].value;
				printf("Player 1 scored!\n");
				printf("Player 1 score = %d\n", player_array[0].score);

				//Reseta a quantidade de vezes que a bola foi rebatida
				ball_rebounce = 0;

				if (player_array[0].score >= 10) {
					printf("Player 1 won!\n");
					mIsRunning = false;
				}

				// Reposiciona a bola no jogo
				ball_array[i].mBallPos.x = rand() % 226 + 768;
				ball_array[i].mBallPos.y = rand() % 768;
				ball_array[i].mBallSpeed.x *= -0.90;
				ball_array[i].mBallSpeed.y *= 0.95;

			}
			// Atualize (negative) a posição da bola se ela colidir com a parede de cima
			// 
			if (ball_array[i].mBallPos.y <= thickness && ball_array[i].mBallSpeed.y < 0.0f)
			{
				ball_array[i].mBallSpeed.y *= -1;
			}
			// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
			// Did the ball collide with the bottom wall?
			else if (ball_array[i].mBallPos.y >= (768 - thickness) && ball_array[i].mBallSpeed.y > 0.0f)
			{
				ball_array[i].mBallSpeed.y *= -1;
			}

		}

	}

}

void GamePong::UpdateGame()
{
	// Espere que 16ms tenham passado desde o último frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time é a diferença em ticks do último frame
	// (convertido pra segundos) - calcula o delta time para atualiza��o do jogo
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// "Clamp" (lima/limita) valor m�ximo de delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// atualize a contagem de ticks par ao pr�ximo frame
	mTicksCount = SDL_GetTicks();
	
	if (playerCount == 1) {
		RunPongSingleplayer(deltaTime);
	}
	else if (playerCount == 2){
		RunPongMultiplayer(deltaTime);
	}
	
}

//Desenhando a tela do jogo
void GamePong::GenerateOutput()
{
	// Setamos a cor de fundo par azul
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		0,	    // B
		255		// A
	);
	
	// limpa o back buffer
	SDL_RenderClear(mRenderer);

	// Desenha as paredes - mudamos a cor de mRenderer para o desenho dos retangulos que formaram as paredes
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo
	
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhamos as outras paredes apenas mudando as coordenadas de wall

	// parede de baixo
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

	// parede da direita
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = 1024;
	wall.h = thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	//como as posi��es da raquete e da bola ser�o atualizadas a cada itera��o do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhando a raquete - usando mPaddlePos que � uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle_1{
		static_cast<int>(player_array[0].mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(player_array[0].mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle_1);

	//desenhando a raquete - usando mPaddlePos que � uma struc de coordenada que foi definida em Game.h
	// 
	if (playerCount == 2)
	{
		SDL_Rect paddle_2{
		static_cast<int>(player_array[1].mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(player_array[1].mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
		};

		SDL_SetRenderDrawColor(mRenderer, 66, 188, 245, 123);

		SDL_RenderFillRect(mRenderer, &paddle_2);

	}
	

	for (int i = 0; i < 10; i++) {

		if (ball_array[i].active) {
			

			//mudar a cor do renderizador para a Ball0
			SDL_SetRenderDrawColor(mRenderer, ball_array[i].red, ball_array[i].green, ball_array[i].blue, 255);

			SDL_Rect Ball{
				static_cast<int>(ball_array[i].mBallPos.x - thickness / 2),
				static_cast<int>(ball_array[i].mBallPos.y - thickness / 2),
				thickness,
				thickness
			};

			SDL_RenderFillRect(mRenderer, &Ball);

		

			SDL_SetRenderDrawColor(mRenderer, ball_array[i].red, ball_array[i].green, ball_array[i].blue, 255);
		}

		

	}
	
	// Desenha a bola especial

	if (Rainball.active) {
			
		//mudar a cor do renderizador para a Ball0
		SDL_SetRenderDrawColor(mRenderer, Rainball.red, Rainball.green, Rainball.blue, 255);

		SDL_Rect SDLRainball{
			static_cast<int>(Rainball.mBallPos.x - thickness / 2),
			static_cast<int>(Rainball.mBallPos.y - thickness / 2),
			thickness,
			thickness
		};

		SDL_RenderFillRect(mRenderer, &SDLRainball);

		

		SDL_SetRenderDrawColor(mRenderer, Rainball.red, Rainball.green, Rainball.blue, 255);
			
	}


	// Desenha os pontos de vida

	if (health > 0 && playerCount == 1) {

		//Desenha o primeiro ponto de vida
		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

		SDL_Rect Health1{
			980,
			728,
			thickness,
			thickness
		};

		SDL_RenderFillRect(mRenderer, &Health1);

		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

		if (health > 1) {
			//Desenha o segundo ponto de vida
			SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

			SDL_Rect Health2{
				980,
				708,
				thickness,
				thickness
			};

			SDL_RenderFillRect(mRenderer, &Health2);

			SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

			if (health > 2) {
				//Desenha o terceiro ponto de vida
				SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

				SDL_Rect Health3{
					980,
					688,
					thickness,
					thickness
				};

				SDL_RenderFillRect(mRenderer, &Health3);

				SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

				if (health > 3) {
					//Desenha o quarto ponto de vida
					SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);

					SDL_Rect Health4{
						980,
						668,
						thickness,
						thickness
					};

					SDL_RenderFillRect(mRenderer, &Health4);

					SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
				}

				if (health > 4) {
					//Desenha o quinto ponto de vida
					SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);

					SDL_Rect Health5{
						980,
						648,
						thickness,
						thickness
					};

					SDL_RenderFillRect(mRenderer, &Health5);

					SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
				}

			}


		}


	}


	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

//Para encerrar o jogo
void GamePong::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
}
