// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int thickness = 15;//sera usado para setar a altura de alguns objetos
const float paddleH = 100.0f;//tamanho da raquete

Game::Game()
:mWindow(nullptr)//para criar uma janela
,mRenderer(nullptr)//para fins de renderização na tela
,mTicksCount(0)//para guardar o tempo decorrido no jogo
,mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
,mPaddleDir(0)//direcao da bolinha
{
	
}

bool Game::Initialize()
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

	mPaddlePos.x = 10.0f;//posição inicial da raquete eixo x
	mPaddlePos.y = 768.0f/2.0f;//posição inicial da raquee eixo y
	//mBallPos.x = 1024.0f/2.0f;//posição da bola eixo x
	//mBallPos.y = 768.0f/2.0f;//posição da bola eixo y
	//mBallVel.x = -200.0f;//velocidade de movimentação da bola no eixo x
	//mBallVel.y = 500.0f;//velocidade de movimentação da bola no eixo y
	
	//Define a vida do jogador
	health = 3;

	//Inicializa as bolas

	for (int i = 0; i < 10; i++) {
		//Define as posições iniciais das bolas
		ball_array[i].pos_x = rand() % 512 + 512;
		ball_array[i].pos_y = rand() % 384 + 384;
		//Define a velocidade inicial da bola
		ball_array[i].speed_x = ((rand() % 100) + ((i + 1) * 100)) * ( - 1);
		ball_array[i].speed_y = rand() % 500;
		//Define a pontuação da bola
		ball_array[i].value = i + 1;
		//Define a pontuação necessária para ativar a bola
		ball_array[i].requirement = (i * 5) * 1.4;
		//Define as cores da bola
		ball_array[i].red = rand() % 255;
		ball_array[i].blue = rand() % 255;
		ball_array[i].green = rand() % 255;
	}

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;//evento, inputs do jogador são armazenados aqui
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
	
	// Get state of keyboard - podemos buscar por alguma tecla específica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys - atualize a direção da raquete com base na entrada do jogador
	// W -> move a raquete para cima, S -> move a raquete para baixo
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Espere que 16ms tenham passado desde o último frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time é a diferença em ticks do último frame
	// (convertido pra segundos) - calcula o delta time para atualização do jogo
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// "Clamp" (lima/limita) valor máximo de delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// atualize a contagem de ticks par ao próximo frame
	mTicksCount = SDL_GetTicks();
	
	// atualiza a posição da raquete
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// CÓDIGO DO PROFESSOR

	/*
	
	// atualiza a posição da bola com base na sua velocidade
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// atualiza a posição da bola se ela colidiu com a raquete
	float diff = mPaddlePos.y - mBallPos.y;
	//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
	//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
	//verifica se a bola está na area da raquete e na mesma posição no eixo x
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// A diferença no eixo y y-difference is small enough
		diff <= paddleH / 2.0f &&
		// Estamos na posicao x correta
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// A bolinha está se movendo para a esquerda
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}

	//Verifica se a bola saiu da tela (no lado esquerdo, onde é permitido)
	//Se sim, encerra o jogo
	// 
	else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}

	// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
	// 
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	
	// Atualize (negative) a posição da bola se ela colidir com a parede de cima
	// 
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}

	// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}

	*/

	/*========================================*/
	/*               Meu Código               */
	/*========================================*/

	//Verifica colisão das bolas

	for (int i = 0; i < 10; i++) {

		if (ball_array[i].requirement <= score) {
			ball_array[i].active = true;
		}

		//printf("Procurando a bola %d\n", i);

		//std::cout << "Estado da bola " << i << " = " << ball_array[i].active << std::endl;

		if (ball_array[i].active) {

			//("Achei uma bola ativa \n\n");

			//printf("Busca a velocidade da bola: \n");
			//printf("Vel. eixo X = %f \nVel. eixo Y = %f\n\n", ball_array[i].speed_x, ball_array[i].speed_y);
			
			//printf("Busca a posicao da bola: \n");
			//printf("Pos. eixo X = %f \nPos. eixo Y = %f\n\n", ball_array[i].pos_x, ball_array[i].pos_y);

			ball_array[i].pos_x += ball_array[i].speed_x * deltaTime;
			ball_array[i].pos_y += ball_array[i].speed_y * deltaTime;

			//printf("Verifica se os valores foram atualizados \n\n");

			//printf("Busca a velocidade da bola: \n");
			//printf("Vel. eixo X = %f \nVel. eixo Y = %f\n\n", ball_array[i].speed_x, ball_array[i].speed_y);

			//printf("Busca a posicao da bola: \n");
			//printf("Pos. eixo X = %f \nPos. eixo Y = %f\n\n", ball_array[i].pos_x, ball_array[i].pos_y);

			// atualiza a posição da bola se ela colidiu com a raquete
			float diff = mPaddlePos.y - ball_array[i].pos_y;
			//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
			//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
			//verifica se a bola está na area da raquete e na mesma posição no eixo x
			diff = (diff > 0.0f) ? diff : -diff;
			if (
				// A diferença no eixo y y-difference is small enough
				diff <= paddleH / 2.0f &&
				// Estamos na posicao x correta
				ball_array[i].pos_x <= 25.0f && ball_array[i].pos_x >= 20.0f &&
				// A bolinha está se movendo para a esquerda
				ball_array[i].speed_x < 0.0f)
			{
				ball_array[i].speed_x *= -1.0f;
			}
			//Verifica se a bola saiu da tela (no lado esquerdo, onde é permitido)
			//Se sim, encerra o jogo
			// ====================
			// FAZER ALTERAÇÃO AQUI
			// ====================
			else if (ball_array[i].pos_x <= 0.0f)
			{
				if (health == 1) {
					printf("Your score was %d\n", score);
					mIsRunning = false;
				
				}
				else {
					printf("Health: %d\n", health);
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
					else if (health == 1) {
						health = 0;
					}
					ball_array[i].pos_x = rand() % 256 + 768;
					ball_array[i].pos_y = rand() % 192 + 576;
					ball_array[i].speed_x *= 0.75;
					ball_array[i].speed_y *= 0.75;
				}
				
			}
			// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
			// 
			else if (ball_array[i].pos_x >= (1024.0f - thickness) && ball_array[i].speed_x > 0.0f)
			{
				float speed_increment = (rand() % 21 - 10) / 100;

				ball_array[i].speed_x *= -1.0f + speed_increment;

				printf("+%d\n", ball_array[i].value);

				score += ball_array[i].value;

				printf("Your score is %d\n", score);

			}
			// Atualize (negative) a posição da bola se ela colidir com a parede de cima
			// 
			if (ball_array[i].pos_y <= thickness && ball_array[i].speed_y < 0.0f)
			{
				ball_array[i].speed_y *= -1;
			}
			// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
			// Did the ball collide with the bottom wall?
			else if (ball_array[i].pos_y >= (768 - thickness) && ball_array[i].speed_y > 0.0f)
			{
				ball_array[i].speed_y *= -1;
			}

		}

		//printf("\n");

	}

	//printf("=======================================\n\n");

	//========================================================================================================================//

}

//Desenhando a tela do jogo
void Game::GenerateOutput()
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
	
	//como as posições da raquete e da bola serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	
	// Código do professor para desenhar a bola

	/*
	//desenhando a bola - usando mBallPos que é uma struc de coordenadas definida como membro em Game.h
	
	//mudar a cor do renderizador para a bola
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);	
	*/

	for (int i = 0; i < 10; i++) {

		if (ball_array[i].active) {
			

			//mudar a cor do renderizador para a Ball0
			SDL_SetRenderDrawColor(mRenderer, ball_array[i].red, ball_array[i].blue, ball_array[i].green, 255);

			SDL_Rect Ball{
				static_cast<int>(ball_array[i].pos_x - thickness / 2),
				static_cast<int>(ball_array[i].pos_y - thickness / 2),
				thickness,
				thickness
			};

			SDL_RenderFillRect(mRenderer, &Ball);

		

			SDL_SetRenderDrawColor(mRenderer, ball_array[i].red, ball_array[i].blue, ball_array[i].green, 255);
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

				SDL_SetRenderDrawColor(mRenderer, 255, 220, 0, 255);

				if (health > 3) {
					//Desenha o quarto ponto de vida
					SDL_SetRenderDrawColor(mRenderer, 255, 220, 0, 255);

					SDL_Rect Health4{
						980,
						668,
						thickness,
						thickness
					};

					SDL_RenderFillRect(mRenderer, &Health4);

					SDL_SetRenderDrawColor(mRenderer, 255, 220, 0, 255);
				}

				if (health > 4) {
					//Desenha o quinto ponto de vida
					SDL_SetRenderDrawColor(mRenderer, 255, 220, 0, 255);

					SDL_Rect Health5{
						980,
						648,
						thickness,
						thickness
					};

					SDL_RenderFillRect(mRenderer, &Health5);

					SDL_SetRenderDrawColor(mRenderer, 255, 220, 0, 255);
				}

			}


		}


	}


	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

//Para encerrar o jogo
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
}
