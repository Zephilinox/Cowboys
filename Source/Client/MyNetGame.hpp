#pragma once
#include <Engine\OGLGame.h>
#include <Engine\Sprite.h>

//SELF
#include "../Architecture/GameData.hpp"
#include "../Architecture/UI/Menu.hpp"

namespace ASGE {
	struct GameTime;
}

/**
*  MyNetGame is the main entrypoint into the game.
*  It is based on the ASGE framework and makes use of
*  the OGLGame class. This allows the rendering and
*  input initialisation to be handled easily and without
*  fuss. The game logic responsible for updating and
*  rendering the game starts here.
*/
class MyNetGame : public ASGE::OGLGame
{
public:
	
	/**
	*  Default constructor for game.
	*/
	MyNetGame() = default;

	/**
	*  The initialisation of the game.
	*  Both the game's initialisation and the API setup
	*  should be performed inside this function. Once the
	*  API is up and running the input system can register
	*  callback functions when certain events happen.
	*/ 
	virtual bool init() override;
	
private:
	/**
	*  The simulation for the game.
	*  The objects in the game need to updated or simulated
	*  over time. Use this function to decouple the render
	*  phases from the simulation.
	*  @param us The ms time between frames and running time
	*  @see GameTime
	*/
	virtual void update(const ASGE::GameTime& gt) override;
	
	/**
	*  The rendering of the game.
	*  This function gets called after the game world has
	*  been updated. Ensure all your objects are in a sane
	*  simulated state before calling this function.
	*  @param us The delta time between frames and running time
	*  @see GameTime
	*/
	virtual void render(const ASGE::GameTime& gt) override;

private:
	std::unique_ptr<GameData> game_data;

	bool capFPS = false;
};

