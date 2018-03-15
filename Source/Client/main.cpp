#include "MyNetGame.h"

//int WINAPI WinMain(
//	HINSTANCE hInstance, HINSTANCE hPrevInstance, 
//	PSTR pScmdline, int iCmdshow)
int main()
{
	MyNetGame game;
	
	if (game.init())
	{
		return game.run();
	}
	
	
		return -1;
	
}