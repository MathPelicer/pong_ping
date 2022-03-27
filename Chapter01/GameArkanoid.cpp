#include "GameArkanoid.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int thickness = 15; //sera usado para setar a altura de alguns objetos
const int spacing = 20; //espaço para separar alguns desenhos
const int blockW = 60; //tamanho dos blocos
const int windowW = 768; //largura da tela
const int windowH = 896; //altura da tela
const float paddleW = 100.0f; //tamanho da raquete


GameArkanoid::GameArkanoid()
	:mWindow(nullptr) //para criar uma janela
	, mRenderer(nullptr) //para fins de renderiza��o na tela
	, mTicksCount(0) //para guardar o tempo decorrido no jogo
	, mIsRunning(true) //verificar se o jogo ainda deve continuar sendo executado
{
	ball_shot = false;
	ball_direction = 1;
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
		windowW,	// Width of window
		windowH,	// Height of window
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

	player_1.mPaddlePos.x = windowW / 2.0f;//posição inicial da raquete eixo x
	player_1.mPaddlePos.y = 810.0f;//posição inicial da raquete eixo y
	player_1.mPaddleReverseWait = 0;
	player_1.mPaddleDir = 0;
	player_1.score = 0;

	// Define a vida do jogador
	health = 3;

	// Define a pontuação inicial do jogador
	player_1.score = 0;

	// Inicializa as bolas
	ball_array[0].draw_only = true;
	for (int i = 0; i < 10; i++) {
		// Define a bola como desativada
		ball_array[i].active = false;
		// Define a velocidade inicial da bola
		ball_array[i].mBallSpeed.x = 200.0f;
		ball_array[i].mBallSpeed.y = -250.0f;
		//Define as cores da bola
		ball_array[i].red = rand() % 200 + 55;
		ball_array[i].blue = rand() % 200 + 55;
		ball_array[i].green = rand() % 200 + 55;
	}

	// Inicializa alguns blocos
	for (int i = 0; i < 5; i++) {
		block_array[i].mBlockPos.x = 2 * spacing + (i * (spacing + blockW / 2.0f)) + (i * 100);
		block_array[i].mBlockPos.y = windowH / 4.0f;
		block_array[i].health = 1;
		block_array[i].value = 20;
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

	// Update paddle direction based on A/D keys - atualize a direção da raquete com base na entrada do jogador
	// A -> move a raquete para esquerda, D -> move a raquete para direita

	// Reseta as variáveis do player
	player_1.mPaddleDir = 0;
	//player_1.mPaddleReverse = false;

	// Verifica qual o input do teclado

	// Input para mover o player para a esquerda
	if (state[SDL_SCANCODE_A])
	{
		player_1.mPaddleDir -= 1;
		ball_direction = -1;
	}
	// Input para mover o player para a direita
	if (state[SDL_SCANCODE_D])
	{
		player_1.mPaddleDir += 1;
		ball_direction = 1;
	}
	// Input para lançar a bola
	if (state[SDL_SCANCODE_SPACE] && !ball_shot)
	{
		ball_shot = true;
		ball_array[0].active = true;
		ball_array[0].draw_only = false;
		ball_array[0].mBallSpeed.x *= ball_direction;
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
		player_1.mPaddlePos.x += player_1.mPaddleDir * 550.0f * deltaTime;
		// verifique que a raquete n�o se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (player_1.mPaddlePos.x < thickness)
		{
			player_1.mPaddlePos.x = thickness;
		}
		else if (player_1.mPaddlePos.x > (windowW - paddleW - thickness))
		{
			player_1.mPaddlePos.x = windowW - paddleW - thickness;
		}
	}

	if (!ball_shot) {
		ball_array[0].mBallPos.x = player_1.mPaddlePos.x + paddleW / 2.0f;
		ball_array[0].mBallPos.y = 780.0f;
		ball_array[0].mBallSpeed.x = 200.0f;
		ball_array[0].mBallSpeed.y = -250.0f;
	}

	/*if (player_1.mPaddleReverse)
	{
		player_1.mPaddlePos.y = 768 - player_1.mPaddlePos.x;
		player_1.mPaddleReverseWait = 50;
	}

	if (player_1.mPaddleReverseWait > 0) {
		player_1.mPaddleReverseWait--;
	}*/

	// ====================
	// FAZER ALTERAÇÃO AQUI
	// ====================

	ball_array[0].mBallPos.x += ball_array[0].mBallSpeed.x * deltaTime;
	ball_array[0].mBallPos.y += ball_array[0].mBallSpeed.y * deltaTime;

	// ====================
	//       COMENTAR
	// ====================

	if ((ball_array[0].mBallPos.y + thickness / 2.0f >= player_1.mPaddlePos.y && ball_array[0].mBallPos.y + thickness < player_1.mPaddlePos.y + thickness) &&
		(ball_array[0].mBallPos.x >= player_1.mPaddlePos.x && ball_array[0].mBallPos.x <= player_1.mPaddlePos.x + paddleW) &&
		ball_array[0].mBallSpeed.y > 0.0f)
	{
		ball_array[0].mBallSpeed.y *= -1.01f;
	}
	//Verifica se a bola saiu da tela (no lado esquerdo, onde é permitido)
	//Se sim, encerra o jogo
	else if (ball_array[0].mBallPos.x <= thickness && ball_array[0].mBallSpeed.x < 0.0f)
	{
		ball_array[0].mBallSpeed.x *= -1.01f;

	}
	// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
	// 
	else if (ball_array[0].mBallPos.x >= (windowW - thickness) && ball_array[0].mBallSpeed.x > 0.0f)
	{
		ball_array[0].mBallSpeed.x *= -1.01f;
	}
	// Atualize (negative) a posição da bola se ela colidir com a parede de cima
	// 
	if (ball_array[0].mBallPos.y <= thickness && ball_array[0].mBallSpeed.y < 0.0f)
	{
		ball_array[0].mBallSpeed.y *= -1.01f;
	}
	// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
	// Did the ball collide with the bottom wall?
	else if (ball_array[0].mBallPos.y >= (player_1.mPaddlePos.y + thickness) && ball_array[0].mBallSpeed.y > 0.0f)
	{
		if (health == 1) {
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
		}

		printf("Health: %d\n", health);

		ball_shot = false;

	}

	// Verifica a colisão com os outros blocos

	for (int i = 0; i < 5; i++) {
		if (block_array[i].health > 0) {
			// Verifica a colisão na parte de baixo do bloco
			if (ball_array[0].mBallPos.y <= block_array[i].mBlockPos.y + thickness &&
				(ball_array[0].mBallPos.x >= block_array[i].mBlockPos.x && ball_array[0].mBallPos.x <= block_array[i].mBlockPos.x + blockW) &&
				ball_array[0].mBallSpeed.y < 0.0f)
			{
				block_array[i].health--;
				ball_array[0].mBallSpeed.y *= -1.01f;
			}
			// Verifica a colisão na parte esquerda do bloco
			else if (ball_array[0].mBallPos.x + thickness >= block_array[i].mBlockPos.x &&
					(ball_array[0].mBallPos.y >= block_array[i].mBlockPos.y 
						&& ball_array[0].mBallPos.y <= block_array[i].mBlockPos.y + thickness) ||
					(ball_array[0].mBallPos.y + thickness >= block_array[i].mBlockPos.y 
						&& ball_array[0].mBallPos.y + thickness <= block_array[i].mBlockPos.y + thickness) &&
					ball_array[0].mBallSpeed.x > 0.0f)
			{
				block_array[i].health--;
				ball_array[0].mBallSpeed.x *= -1.01f;
			}
			// Verifica a colisão na parte de cima do bloco
			else if (ball_array[0].mBallPos.y + thickness >= block_array[i].mBlockPos.y &&
					(ball_array[0].mBallPos.x >= block_array[i].mBlockPos.x && ball_array[0].mBallPos.x <= block_array[i].mBlockPos.x + blockW) &&
					ball_array[0].mBallSpeed.y > 0.0f)
			{
				block_array[i].health--;
				ball_array[0].mBallSpeed.y *= -1.01f;
			}
			// Verifica a colisão na parte direita do bloco**************
			else if (ball_array[0].mBallPos.x + thickness <= block_array[i].mBlockPos.x &&
					(ball_array[0].mBallPos.y >= block_array[i].mBlockPos.y
						&& ball_array[0].mBallPos.y <= block_array[i].mBlockPos.y + thickness) ||
					(ball_array[0].mBallPos.y + thickness >= block_array[i].mBlockPos.y
						&& ball_array[0].mBallPos.y + thickness <= block_array[i].mBlockPos.y + thickness) &&
					ball_array[0].mBallSpeed.x < 0.0f)
			{
				block_array[i].health--;
				ball_array[0].mBallSpeed.x *= -1.01f;
			}

			if (block_array[i].health == 0) {
				player_1.score += block_array[i].value;
				printf("+%d\n", block_array[i].value);

				player_1.score += ball_array[i].value;

				printf("Your score is %d\n", player_1.score);
			}
		}
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
	SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, 255);

	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		windowW,	// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

	//desenhamos as outras paredes apenas mudando as coordenadas de wall

	// parede de baixo
	wall.y = 896 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	// parede da direita
	wall.x = windowW - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = windowH;
	SDL_RenderFillRect(mRenderer, &wall);

	// parede da esquerda
	wall.x = 0;
	wall.y = 0;
	wall.w = thickness;
	wall.h = windowH;
	SDL_RenderFillRect(mRenderer, &wall);

	//como as posições da raquete e da bola serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle{
		static_cast<int>(player_1.mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(player_1.mPaddlePos.y),
		static_cast<int>(paddleW),
		thickness
	};
	SDL_RenderFillRect(mRenderer, &paddle);


	for (int i = 0; i < 10; i++) {

		if (ball_array[i].active || ball_array[i].draw_only) {


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

		SDL_Rect Health{
			spacing,
			player_1.mPaddlePos.y + 2 * spacing - 5,
			thickness,
			thickness
		};

		SDL_RenderFillRect(mRenderer, &Health);

		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

		if (health > 1) {
			//Desenha o segundo ponto de vida

			Health.x += spacing;

			SDL_RenderFillRect(mRenderer, &Health);

			if (health > 2) {
				//Desenha o terceiro ponto de vida

				Health.x += spacing;

				SDL_RenderFillRect(mRenderer, &Health);

				if (health > 3) {
					//Desenha o quarto ponto de vida

					Health.x += spacing;

					SDL_RenderFillRect(mRenderer, &Health);

				}

				if (health > 4) {
					//Desenha o quinto ponto de vida

					Health.x += spacing;

					SDL_RenderFillRect(mRenderer, &Health);
				}

			}


		}


	}

	// Desenha os blocos

	for (int i = 0; i < 5; i++) {
		if (block_array[i].health != 0) {
			//Desenha o primeiro ponto de vida
			SDL_SetRenderDrawColor(mRenderer, 252, 186, 3, 255);

			SDL_Rect Block{
				block_array[i].mBlockPos.x,
				block_array[i].mBlockPos.y,
				blockW,
				thickness
			};

			SDL_RenderFillRect(mRenderer, &Block);

			SDL_SetRenderDrawColor(mRenderer, 252, 186, 3, 255);
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