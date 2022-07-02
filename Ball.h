#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <vector>
#include <Game.h>
namespace Pong
{
	class Ball
	{
	public:
		Ball(std::pair<unsigned, unsigned>* _score = new std::pair<unsigned, unsigned>(), Uint64* _frames=0, bool mode = false, int _WIDTH = 1080, int _HEIGHT = 720);
		void wallCollision(bool audio=true);
		void playerCollision(SDL_Rect* rect, int num);
		void move(SDL_Rect* player1, SDL_Rect* player2);
		void move();
		void render(SDL_Renderer* gRenderer);
		void reset();
		void loadFX();
		SDL_Rect* getRect();
		const void Report();
		const std::vector<int> getVels() { return std::vector<int>{xvel, yvel, xoffset, yoffset}; }
	private:
		const int distance(SDL_Rect* rect1, SDL_Rect* rect2) const;
		int xvel, yvel, xoffset, yoffset;
		SDL_Rect rect;
		std::pair<unsigned, unsigned>* score;
		Uint64* frames;
		int HEIGHT;
		int WIDTH;
		Mix_Chunk* collide;
		Mix_Chunk* point;

		static const int offsetcap = 50; //should stay >0
		bool offsetMode = false;
	};

	void Ball::loadFX()
	{
		collide = Mix_LoadWAV("Assets/Collide.wav");
		point = Mix_LoadWAV("Assets/Point.wav");
		if (collide == NULL || point == NULL) 
		{
			std::cout << Mix_GetError() << "\n";
		}
	}

	SDL_Rect* Ball::getRect()
	{
		return &rect;
	}

	Ball::Ball(std::pair<unsigned, unsigned>* _score, Uint64* _frames, bool mode, int _WIDTH, int _HEIGHT)
	{
		using namespace Pong;
		srand(time(0));
		xvel = rand() % 15;
		yvel = rand() % 15;
		xoffset = 1;
		yoffset = 1;
		WIDTH = _WIDTH;
		HEIGHT = _HEIGHT;
		rect = { (WIDTH / 2) - (40 / 2), (HEIGHT / 2) - (40 / 2), 40, 40}; //WIDTH / 2 = center of screen's x, -40/2 = half of the width, thus snapping the rect's x value to the center of the screen (same applies to y-value)
		score = _score;
		offsetMode = mode;
		frames = _frames;
	}

	void Ball::reset()
	{
		xvel = rand() % 15;
		yvel = rand() % 15;
		rect.x = (WIDTH / 2) - (rect.w / 2);
		rect.y = (HEIGHT / 2) - (rect.h / 2);
		xoffset = 1;
		yoffset = 1;
	}

	void Ball::wallCollision(bool audio)
	{
		if (rect.x + rect.w >= WIDTH)
		{
			reset();
			score->first++;
			if (audio)
			{
				Mix_PlayChannel(-1, point, 0);
			}
			//std::cout << score->first << " - " << score->second << "\n";
		}
		else if (rect.x <= 0)
		{
			
			reset();
			score->second++;
			if (audio)
			{
				Mix_PlayChannel(-1, point, 0);
			}
			//std::cout << score->first << " - " << score->second << "\n";
		}
		if (rect.y + rect.h >= HEIGHT)
		{
			//xvel *= -1;
			yvel *= -1;
			yoffset *= -1;
			if (audio)
			{
				Mix_PlayChannel(-1, collide, 0);
			}
		}
		else if (rect.y <= 0)
		{
			//xvel *= -1;
			yvel *= -1;
			yoffset *= -1;
			if (audio)
			{
				Mix_PlayChannel(-1, collide, 0);
			}
		}
	}

	void Ball::move()
	{
		if (rect.x + xvel + xoffset < 0) { rect.x = 0; }
		else if (rect.x + xvel + xoffset + rect.w > WIDTH) { rect.x = WIDTH - rect.w; }
		else { rect.x += xvel + xoffset; }

		if (rect.y + yvel + yoffset + rect.h > HEIGHT) { rect.y = HEIGHT - rect.h; }
		else if (rect.y + yvel + yoffset < 0) { rect.y = 0; }
		else { rect.y += yvel + yoffset; }
	}

	void Ball::move(SDL_Rect* player1, SDL_Rect* player2)
	{
		if (rect.x + xvel + xoffset < 0) { rect.x = 0; }
		else if (rect.x + xvel + xoffset + rect.w > WIDTH) { rect.x = WIDTH - rect.w; }
		else { rect.x += xvel + xoffset; }

		if (rect.y + yvel + yoffset + rect.h > HEIGHT) { rect.y = HEIGHT - rect.h; }
		else if (rect.y + yvel + yoffset < 0) { rect.y = 0; }
		else { rect.y += yvel + yoffset; }

		SDL_Rect intersect;
		if (SDL_IntersectRect(&rect, player1, &intersect))
		{
			Mix_PlayChannel(-1, collide, 0);
			rect.x += intersect.w;
			xvel *= -1;
			xoffset *= -1;
			if (offsetMode && xoffset < offsetcap && xoffset > -offsetcap && yoffset < offsetcap && yoffset > -offsetcap)
			{
				xoffset += ((rand() % 10) + 1) * std::abs(xoffset) / xoffset;
				rect.y -= intersect.h;
			}
		}
		else if (SDL_IntersectRect(&rect, player2, &intersect))
		{
			Mix_PlayChannel(-1, collide, 0);
			rect.x -= intersect.w;
			xvel *= -1;
			xoffset *= -1;
			if (offsetMode && xoffset < offsetcap && xoffset > -offsetcap && yoffset < offsetcap && yoffset > -offsetcap)
			{
				xoffset += ((rand() % 10) + 1) * std::abs(xoffset) / xoffset;
				yoffset += ((rand() % 10) + 1) * std::abs(yoffset) / yoffset;
				rect.y -= intersect.h;
			}
		}
	}

	void Ball::playerCollision(SDL_Rect* player, int num)
	{
		if (distance(&rect, player) < 100)
		{
			if (SDL_HasIntersection(&rect, player) == SDL_TRUE)
			{
				xvel *= -1;
				yvel *= -1;

				if (offsetMode && xoffset < offsetcap && xoffset > -offsetcap && yoffset < offsetcap && yoffset > -offsetcap)
				{
					xoffset += ((rand() % 10) + 1) * std::abs(xoffset) / xoffset;
					yoffset += ((rand() % 10) + 1) * std::abs(yoffset) / yoffset;
				}
			}
		}
	}

	const int Ball::distance(SDL_Rect* rect1, SDL_Rect* rect2) const
	{
		return (int)floor(sqrt(std::pow(rect2->y - rect1->y, 2) + std::pow(rect2->x - rect1->x, 2)));
	}

	void Ball::render(SDL_Renderer* gRenderer)
	{
		SDL_RenderFillRect(gRenderer, &rect);
	}

	const void Ball::Report()
	{
		using std::cout;
		cout << "\n====================\n";
		cout << "type: Ball\n" << "x: " << rect.x << "\ny: " << rect.y << "\nx vel: " << xvel << "\ny vel: " << yvel << "\nx offset: " << xoffset << "\ny offset: " << yoffset;
		cout << "\nscore: " << score->first << " - " << score->second << "\nframes: " << *frames;
		cout << "\n====================\n";
	}
}