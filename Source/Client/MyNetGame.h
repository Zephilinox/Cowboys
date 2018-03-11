#pragma once
#include <Engine\OGLGame.h>
#include <Engine\Sprite.h>
#include <Client\Networking.h>

//SELF
#include "../Architecture/GameData.hpp"

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
class MyNetGame 
	: public ASGE::OGLGame
{
public:
	
	/**
	*  Default constructor for game.
	*/
	MyNetGame() = default;
	
	/**
	*  Destructor for game.
	*  Make sure to clean up any dynamically allocated
	*  memory inside the game's destructor. For example
	*  game fonts need to be deallocated.
	*/ 
	~MyNetGame();

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
	virtual void update(const ASGE::GameTime& ms) override;
	
	/**
	*  The rendering of the game.
	*  This function gets called after the game world has
	*  been updated. Ensure all your objects are in a sane
	*  simulated state before calling this function.
	*  @param us The delta time between frames and running time
	*  @see GameTime
	*/
	virtual void render(const ASGE::GameTime& ms) override;

	/**
	*  The key handling function for the game.
	*  Key inputs will be delivered and handled within this function.
	*  Make a decision whether to process the input immediately
	*  or whether to generate a queue of actions that are then
	*  processed at the beginning of the game update loop.
	*  @param data They key event and its related data.
	*  @see SharedEventData
	*/
	void keyHandler(const ASGE::SharedEventData data);

private:
	
	std::thread th;           /**< Network Event Thread. 
							       Consumes incoming network events. */
	ClientComponent network;  /**< Network Component. 
							       The networking component used for clients. */
	int key_handler_id = -1;  /**< Input Callback ID. 
							       The callback ID assigned by the game engine. */

	std::unique_ptr<GameData> game_data;
};

