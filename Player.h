#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <AI.h>
namespace Pong
{

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_STOP 2

#define PLAYER_PLAYER 0
#define PLAYER_AI 1

	class User
	{
	public:
		virtual const void Move() = NULL;
		virtual const void Render(SDL_Renderer* gRenderer) = NULL;
	}; // User interface

	class Player : User
	{
	public:
		Player(SDL_Rect loc, int player = 0, int _WIDTH=1080, int _HEIGHT=720) // 0 = player 1 in Game.h
		{
			dir = DIR_STOP;
			rect = loc;
			playerNum = player;
			if (player == 0) { keys = {SDLK_w, SDLK_s}; }
			else if (player == 1) { keys = { SDLK_UP, SDLK_DOWN }; }
			HEIGHT = _HEIGHT;
			WIDTH = _WIDTH;
		}
		
		SDL_Rect rect;
		int playerNum;
		void ChangeDir(SDL_Event &e)
		{
			if (e.type == SDL_KEYUP) { dir = DIR_STOP; }
			else if (e.key.keysym.sym == keys.first) { dir = DIR_UP; }
			else if (e.key.keysym.sym == keys.second) { dir = DIR_DOWN; }
		}
		const void Move()
		{
			switch (dir)
			{
			case DIR_UP:
				if (rect.y > 0) 
				{
					rect.y -= Speed; // less y is up}
				}
				break;
			case DIR_DOWN:
				if (rect.y < HEIGHT - rect.h)
				{
					rect.y += Speed;
				}
				break;
			}
		}
		const void Render(SDL_Renderer* gRenderer) { SDL_RenderFillRect(gRenderer, &rect); }
		const void Report()
		{
			using std::cout;
			cout << "\n====================\n";
			cout << "type: Player\n" << "player: " << playerNum + 1 << "\ny: " << rect.x << "\ndir: " << dir;
			cout << "\n====================\n";
		}
	private:
		int dir;
		static const int Speed = 30;
		std::pair<int, int> keys;
		int WIDTH, HEIGHT;
	} ;

	struct PlayerContainer : User
	{
		Player *player;
		AI *ai;
		int type;
		SDL_Rect* rect;
		int playerNum;

		void eventFunc(SDL_Event &e)
		{
			if (type == PLAYER_PLAYER)
			{
				player->ChangeDir(e);
			}
		}
		void physicsFunc(Ball* _ball)
		{
			if (type == PLAYER_AI)
			{
				ai->calcCollidePos(_ball);
			}
		}
		const void Move()
		{
			if (type == PLAYER_PLAYER)
			{
				player->Move();
			}
			else if (type == PLAYER_AI)
			{
				ai->Move();
			}
		}
		const void Render(SDL_Renderer* gRenderer)
		{
			if (type == PLAYER_PLAYER) { player->Render(gRenderer); }
			else if (type == PLAYER_AI) { ai->Render(gRenderer); }
		}
		const void Report()
		{
			if (type == PLAYER_PLAYER) { player->Report(); }
			else if (type == PLAYER_AI) { ai->Report(); }
		}

		PlayerContainer(Ball* _ball, SDL_Rect loc, int _player, int _type, int _WIDTH= 1080, int _HEIGHT=720)
		{
			playerNum = _player;
			type = _type;
			if (type == PLAYER_AI)
			{
				//std::cout << "New AI Created\n";
				ai = new AI(_ball, loc, _player);
				//player = nullptr;
				rect = &ai->rect;
			}
			else if (type == PLAYER_PLAYER)
			{
				//std::cout << "New Player Created\n";
				player = new Player(loc, _player, _WIDTH, _HEIGHT);
				//ai = nullptr;
				rect = &player->rect;
			}
		}
	};
}

