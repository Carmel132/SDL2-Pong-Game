#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <vector>
#include <Player.h>
#include <Ball.h>
namespace Pong
{
#define PLAYER1 0
#define PLAYER2 1
	class AI
	{
	public:
		AI(Ball* ball, SDL_Rect loc, int player);

		SDL_Rect rect;
		int playerNum;

		const void Render(SDL_Renderer* gRenderer);
		const void Move();
		void calcCollidePos(Ball* _ball);
		const void Report();
	private:
		Ball ball;
		int collide; // can be represented as a y value rather than a (x, y) coordinate
		static const int Speed = 30;
		int xcollide;
	};

	AI::AI(Ball* _ball, SDL_Rect loc, int player)
	{
		rect = loc;
		playerNum = player;
		ball = *_ball;
		collide = -1;
		if (playerNum == PLAYER1) { xcollide = rect.x + rect.w; }
		else if (playerNum == PLAYER2) { xcollide = rect.x; }
	}

	void AI::calcCollidePos(Ball* _ball)
	{
		ball = *_ball;

		SDL_Rect* ballRect = ball.getRect();
		if (playerNum == PLAYER1 && ball.getVels()[0] + ball.getVels()[2] < 0)
		{
			while (ballRect->x > xcollide)
			{
				ball.wallCollision(false);
				ball.move();
			}
			collide = ballRect->y + (ballRect->h / 2);
		}
		else if (playerNum == PLAYER2 && ball.getVels()[0] + ball.getVels()[2] > 0)
		{
			while (ballRect->x + ballRect->w < xcollide)
			{
				ball.wallCollision(false);
				ball.move();
			}
			collide = ballRect->y + (ballRect->h / 2);
		}
	}

	const void AI::Render(SDL_Renderer* gRenderer)
	{
		SDL_RenderFillRect(gRenderer, &rect);
	}

	const void AI::Move()
	{
		if (collide == -1) { return; }

		if (collide + Speed < rect.y + (rect.h/2)) { rect.y -= Speed; }
		else if (collide - Speed > rect.y + (rect.h / 2)) { rect.y += Speed; }
	}

	const void AI::Report()
	{
		using std::cout;
		cout << "\n====================\n";
		cout << "type: AI\n" << "player: " << playerNum + 1 << "\nx: " << rect.x << "\ny: " << rect.y << "\ncollide: " << collide;
		cout << "\n====================\n";
	}
}

