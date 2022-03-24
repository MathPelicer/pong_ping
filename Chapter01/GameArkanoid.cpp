#include "GameArkanoid.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int thickness = 15;//sera usado para setar a altura de alguns objetos
const float paddleH = 100.0f;//tamanho da raquete

GameArkanoid::GameArkanoid()
	:mWindow(nullptr)//para criar uma janela
	, mRenderer(nullptr)//para fins de renderiza��o na tela
	, mTicksCount(0)//para guardar o tempo decorrido no jogo
	, mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
{

}

bool GameArkanoid::Initialize()
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
		768,	// Width of window
		1024,	// Height of window
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

	player_1.mPaddlePos.x = 768.0f / 2.0f;//posição inicial da raquete eixo x
	player_1.mPaddlePos.y = 30.0f;//posição inicial da raquete eixo y
	player_1.mPaddleReverseWait = 0;
	player_1.mPaddleDir = 0;
	player_1.score = 0;

	// Define a vida do jogador
	health = 3;

	// Define a pontuação inicial do jogador
	player_1.score = 0;

	// Inicializa as bolas

	for (int i = 0; i < 10; i++) {
		// Define a velocidade inicial da bola
		ball_array[i].mBallSpeed.x = ((rand() % 100) + ((i + 1) * 100)) * (-1);
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

void GameArkanoid::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void GameArkanoid::ProcessInput()
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

	// Reseta as variáveis do player
	player_1.mPaddleDir = 0;
	player_1.mPaddleReverse = false;

	// Verifica qual o input do teclado

	// Input para "subir" o player
	if (state[SDL_SCANCODE_D])
	{
		player_1.mPaddleDir -= 1;
	}
	// Input para "descer" o player
	if (state[SDL_SCANCODE_A])
	{
		player_1.mPaddleDir += 1;
	}
	// Input para reverter a raquete do player
	/*if (state[SDL_SCANCODE_SPACE] && player_1.mPaddleReverseWait == 0)
	{
		player_1.mPaddleReverse = true;
	}*/
	// Fecha o jogo
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
}

void GameArkanoid::RunArkanoid(float deltaTime) {
	// atualiza a posição da raquete
	if (player_1.mPaddleDir != 0)
	{
		player_1.mPaddlePos.x += player_1.mPaddleDir * 300.0f * deltaTime;
		// verifique que a raquete n�o se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (player_1.mPaddlePos.x < (paddleH / 2.0f + thickness))
		{
			player_1.mPaddlePos.x = paddleH / 2.0f + thickness;
		}
		else if (player_1.mPaddlePos.x > (768.0f - paddleH / 2.0f - thickness))
		{
			player_1.mPaddlePos.x = 768.0f - paddleH / 2.0f - thickness;
		}
	}



	if (player_1.mPaddleReverse)
	{
		player_1.mPaddlePos.y = 768 - player_1.mPaddlePos.x;
		player_1.mPaddleReverseWait = 50;
	}

	if (player_1.mPaddleReverseWait > 0) {
		player_1.mPaddleReverseWait--;
	}
}

void GameArkanoid::UpdateGame()
{
	// Espere que 16ms tenham passado desde o �ltimo frame - limitando os frames
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

	RunArkanoid(deltaTime);

}

//Desenhando a tela do jogo
void GameArkanoid::GenerateOutput()
{
	// Setamos a cor de fundo para preto
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
		768,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhamos as outras paredes apenas mudando as coordenadas de wall

	// parede de baixo
	wall.y = 1024 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

	// parede da direita
	wall.x = 768 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	// parede da esquerda
	wall.x = thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	//como as posições da raquete e da bola serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle{
		static_cast<int>(player_1.mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(player_1.mPaddlePos.y - paddleH / 2),
		static_cast<int>(paddleH),
		thickness
	};
	SDL_RenderFillRect(mRenderer, &paddle);


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

	// Desenha os pontos de vida

	if (health > 0) {

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
void GameArkanoid::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
}