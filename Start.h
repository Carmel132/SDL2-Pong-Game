#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <vector>
#include <tuple>
#include <algorithm>
// The clean code ends here
namespace Pong
{
	class Start // needs to return both player types, offset cap, 
	{
	public:

		std::tuple<int, int, int> start(SDL_Renderer* gRenderer, int WIDTH, int HEIGHT, SDL_Window* gWindow, TTF_Font* font);
	private:
	};

	class SettingsBar
	{
	public:
		SettingsBar(SDL_Rect _rect, int _max);

		const void render(SDL_Renderer* gRenderer);
		int x;
		int getVal();
		bool isSelecting(SDL_Point point);
		void move(int newx);
	private:
		SDL_Rect rect;
		SDL_Rect circlerect;
		const int Radius = 40;
		void DrawCircle(SDL_Renderer* gRenderer, int centreX, int centreY, int radius);
		int max;
	};

	class IButton // Button interface
	{
	public:
		SDL_Rect* rect = 0;
		TTF_Font* font = 0;
		std::string text = "";

		virtual void Render(SDL_Renderer* gRenderer) = 0;
		virtual void Clicked(SDL_Point point) = 0;
	};
	
	class StartButton : IButton
	{
	public:
		StartButton(SDL_Rect _rect, TTF_Font* _font, bool* quit);

		void Render(SDL_Renderer* gRenderer);
		void Clicked(SDL_Point point);
	private:
		SDL_Rect* rect;
		TTF_Font* font;
		std::string text;
		bool* quit;
	};
	
	class PlayerButton : IButton
	{
	public:
		PlayerButton(SDL_Rect _rect, TTF_Font* _font, bool* _player);

		void Render(SDL_Renderer* gRenderer);
		void Clicked(SDL_Point point);
	private:
		SDL_Rect* rect;
		TTF_Font* font;
		std::string text;
		bool* player;
	};

	PlayerButton::PlayerButton(SDL_Rect _rect, TTF_Font* _font, bool* _player)
	{
		rect = &_rect;
		font = _font;
		player = _player;
		text = "Player";
	}

	void PlayerButton::Clicked(SDL_Point point)
	{
		if (SDL_PointInRect(&point, rect))
		{
			text = text[0] == 'P' ? "AI" : "Player";
			*player = !*player;
		}
	}

	void PlayerButton::Render(SDL_Renderer* gRenderer)
	{
		//SDL_RenderDrawRect(gRenderer, rect);
		SDL_RenderCopy(gRenderer, SDL_CreateTextureFromSurface(gRenderer, TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255 })), NULL, rect);
	}

	StartButton::StartButton(SDL_Rect _rect, TTF_Font* _font, bool* _quit)
	{
		rect = &_rect;
		font = _font;
		text = "Start";
		quit = _quit;
	}

	void StartButton::Render(SDL_Renderer* gRenderer)
	{
		//SDL_RenderDrawRect(gRenderer, rect);
		SDL_RenderCopy(gRenderer, SDL_CreateTextureFromSurface(gRenderer, TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255 })), NULL, rect);
	}

	void StartButton::Clicked(SDL_Point point)
	{
		if (SDL_PointInRect(&point, rect))
		{
			*quit = true;
		}
	}

	void SettingsBar::move(int newx)
	{
		if (newx <= rect.x) { x = rect.x; }
		else if (newx > rect.x + rect.w) { x = rect.x + rect.w; }
		else { x = newx; }
		circlerect = { x - Radius, rect.y + (rect.h / 2) - Radius, Radius * 2, Radius * 2 };
	}

	bool SettingsBar::isSelecting(SDL_Point point)
	{
		return SDL_PointInRect(&point, &circlerect);
	}

	int SettingsBar::getVal()
	{
		return (x * max) / (rect.x + rect.w);
	}

	SettingsBar::SettingsBar(SDL_Rect _rect, int _max)
	{
		rect = _rect;
		max = _max;
		x = (int)std::floor(rect.x / 2);
		circlerect = { x - Radius, rect.y + (rect.h / 2) - Radius, Radius *2, Radius * 2};
	}

	const void SettingsBar::render(SDL_Renderer* gRenderer)
	{
		SDL_RenderFillRect(gRenderer, &rect);
		DrawCircle(gRenderer, x, rect.y + (rect.h / 2), Radius);
		//SDL_RenderDrawRect(gRenderer, &circlerect);
	}

	void SettingsBar::DrawCircle(SDL_Renderer* gRenderer, int x, int y, int radius)
	{
		int offsetx, offsety, d;
		int status;


		offsetx = 0;
		offsety = radius;
		d = radius - 1;
		status = 0;

		while (offsety >= offsetx) {

			status += SDL_RenderDrawLine(gRenderer, x - offsety, y + offsetx,
				x + offsety, y + offsetx);
			status += SDL_RenderDrawLine(gRenderer, x - offsetx, y + offsety,
				x + offsetx, y + offsety);
			status += SDL_RenderDrawLine(gRenderer, x - offsetx, y - offsety,
				x + offsetx, y - offsety);
			status += SDL_RenderDrawLine(gRenderer, x - offsety, y - offsetx,
				x + offsety, y - offsetx);

			if (status < 0) {
				status = -1;
				break;
			}

			if (d >= 2 * offsetx) {
				d -= 2 * offsetx + 1;
				offsetx += 1;
			}
			else if (d < 2 * (radius - offsety)) {
				d += 2 * offsety - 1;
				offsety -= 1;
			}
			else {
				d += 2 * (offsety - offsetx - 1);
				offsety -= 1;
				offsetx += 1;
			}
		}
	}

	std::tuple<int, int, int> Start::start(SDL_Renderer* gRenderer, int WIDTH, int HEIGHT, SDL_Window* gWindow, TTF_Font* font)
	{
		bool quit = false, player1 = true, player2 = true;

		SDL_Event e;
		// magic numbers they said; it'll be fun they said
		// here i am
		// trying to decipher what the hell WIDTH / 2 - WIDTH / 8 means, or why i'm multiplying HEIGHT by 3/8
		SettingsBar offset = SettingsBar({100, HEIGHT - 100, WIDTH - 100 * 2, 20}, 150);
		StartButton startbutton = StartButton({WIDTH / 2 - WIDTH / 8, (3 * HEIGHT) / 8, WIDTH / 4, HEIGHT / 8}, font, &quit);
		PlayerButton leftbutton = PlayerButton({WIDTH / 12 - 100, HEIGHT / 2 - HEIGHT / 6 - 25, WIDTH / 3, HEIGHT / 3}, font, &player1);
		PlayerButton rightbutton = PlayerButton({WIDTH - (WIDTH / 12) - WIDTH / 3 + 100, HEIGHT / 2 - HEIGHT / 6 - 25, WIDTH / 3, HEIGHT / 3}, font, &player2);

		bool selectingOffset = false;

		int mousex, mousey;

		while (!quit)
		{
			SDL_GetMouseState(&mousex, &mousey);
			offset.getVal();
			// event loop
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					//cout << "Quitting" << "\n";
					quit = true;
					SDL_Quit();
				}
				else if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
						SDL_Quit();
					}
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					if (offset.isSelecting({ mousex, mousey }))
					{
						selectingOffset = true;
					}
					startbutton.Clicked({ mousex, mousey });
					leftbutton.Clicked({ mousex, mousey });
					rightbutton.Clicked({ mousex, mousey });
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
					selectingOffset = false;
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					if (selectingOffset)
					{
						offset.move(mousex);
					}
				}
			}


			// physics loop

			// render loop    
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(gRenderer);
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			offset.render(gRenderer);
			startbutton.Render(gRenderer);
			leftbutton.Render(gRenderer);
			rightbutton.Render(gRenderer);

			SDL_RenderPresent(gRenderer);
			SDL_UpdateWindowSurface(gWindow);
		}
		return std::tuple<int, int, int>{!player1, !player2, offset.getVal()};
	}

}