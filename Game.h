#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <Ball.h>
#include <Player.h>
#include <AI.h>
namespace Pong
{
#define PLAYER1 0
#define PLAYER2 1
extern const int WIDTH = 1980;
extern const int HEIGHT = 1080;


#define PLAYER1START { 60, 160, 40, 160 } // 60 pixels from left wall, 160 from ceiling, 40 wide and 160 tall
#define PLAYER2START { WIDTH - 60 - 40, 160, 40, 160 } // 60 from wall (-40 accounts for width), 160 from ceiling, 40 wide and 160 tall


	
	extern const int PLAYER = 0;
	extern const int AI_PLAYER = 1;

	class Game
	{
	public:
		void run(std::pair<int, int> players, bool offset = false);
	private:
		SDL_Renderer* gRenderer = nullptr;
		static const int SCREEN_WIDTH = WIDTH;
		static const int SCREEN_HEIGHT = HEIGHT;
		SDL_Window* gWindow = nullptr;
		SDL_Rect* intersect = nullptr;
		SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
		SDL_Rect textRect = {182 +20, 160/3 - 40, WIDTH - 182*2, HEIGHT/2};
		TTF_Font* font = nullptr;
		std::pair<unsigned, unsigned> score;
		SDL_Rect midline = { WIDTH / 2 - 50 / 2, 0, 50, HEIGHT };

		bool init();
	};

	bool Game::init()
	{
		bool success = true;
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
			success = false;
		}

		else
		{
			gWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (gWindow == NULL)
			{
				printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Create renderer for window
				gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
				if (gRenderer == NULL)
				{
					printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
					success = false;
				}
				else
				{

					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

					//Initialize PNG loading
					int imgFlags = IMG_INIT_PNG;
					if (!(IMG_Init(imgFlags) & imgFlags))
					{
						printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
						success = false;
					}
					if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 2048, NULL, SDL_AUDIO_ALLOW_FORMAT_CHANGE) < 0)
					{
						printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
						success = false;
					}
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
				}
			}
		}
		return success;
	}

	void Game::run(std::pair<int, int> players, bool offset)
	{
		using std::cout;

		Uint64 frames = 0;
		bool quit = false;
		SDL_Event e;
		Ball ball = Ball(&score,&frames, offset, WIDTH, HEIGHT);
		PlayerContainer* player1 = new PlayerContainer(&ball, PLAYER1START, PLAYER1, players.first, WIDTH, HEIGHT);
		PlayerContainer* player2 = new PlayerContainer(&ball, PLAYER2START, PLAYER2, players.second, WIDTH, HEIGHT);
		
		

		if (init())
		{
			ball.loadFX();
			font = TTF_OpenFont("Assets/8-bit.ttf", 66);

			if (font == NULL) { std::cout << TTF_GetError(); }
			while (!quit)
			{
				Uint64 start = SDL_GetPerformanceCounter();
				


				// event loop
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						//cout << "Quitting" << "\n";
						quit = true;
						SDL_Quit();
					}
					player1->eventFunc(e);
					player2->eventFunc(e);
					

				}

				// physics loop
				player2->physicsFunc(&ball);
				player1->physicsFunc(&ball);
				player1->Move();
				player2->Move();
				ball.wallCollision();
				ball.playerCollision(player1->rect, player1->playerNum);
				ball.playerCollision(player2->rect, player2->playerNum);
				ball.move(player1->rect, player2->rect);

				// debug section
				if (frames % 12 == 0) // every 12 frames, send log to stdout
				{
					//for (int i = 0; i < 30; i++) { std::cout << "\n"; } // stdout is 30 lines big
					//player1->Report();
					//player2->Report();
					//ball.Report();
				}
				if (frames % 5000 == 0) // every 5000 frames, clear stdout
				{
					//system("CLS");
				}
				

				// render section
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(gRenderer);
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				player1->Render(gRenderer);
				player2->Render(gRenderer);
				ball.render(gRenderer);
				//SDL_RenderCopy( gRenderer, mTexture, NULL, &rect ); (render a texture)

				std::string message = (std::to_string(score.first) + "   " + std::to_string(score.second));
				//std::cout << message << '\n';
				SDL_RenderCopy(gRenderer, SDL_CreateTextureFromSurface(gRenderer, TTF_RenderText_Solid(font, message.c_str(), {255, 255, 255})), NULL, &textRect);
				SDL_RenderFillRect(gRenderer, &midline);

				SDL_RenderPresent(gRenderer);
				SDL_UpdateWindowSurface(gWindow);
				// fps cap
				Uint64 end = SDL_GetPerformanceCounter();
				float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency();
				SDL_Delay(floor(16.666f - elapsedMS));

				frames++;
			}
		}
	}
}
