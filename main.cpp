#include <iostream>
#include <Game.h>
#include <Player.h>
#include <Ball.h>
#include <AI.h>

int main(int argc, char* argv[])
{
	using std::cout;
	using namespace Pong;
	Game* game = new Game();
	game->run({AI_PLAYER, AI_PLAYER}, true);
	
	game = NULL;
	cout << sizeof(game);

	//cout << "Process Terminated\n";
	return 0;
}
