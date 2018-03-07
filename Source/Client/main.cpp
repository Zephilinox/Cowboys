#include "MyNetGame.h"

//int WINAPI WinMain(
//	HINSTANCE hInstance, HINSTANCE hPrevInstance, 
//	PSTR pScmdline, int iCmdshow)
int main()
{
	std::unique_ptr<MyNetGame> game = 
		std::make_unique<MyNetGame>();
	
	if (game->init())
	{
		return game->run();
	}

	// game failed to initialise
	else
	{
		return -1;
	}
}