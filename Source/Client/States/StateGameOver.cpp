#include "../States/StateGameOver.h"

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/Timer.hpp"
#include "../../Architecture/Constants.hpp"

StateGameOver::StateGameOver(GameData* game_data, bool isWinner)
	: State(game_data, true, false)
	, menu(game_data)
	, dim_background(game_data->getRenderer()->createRawSprite())
	, win_splash(game_data->getRenderer()->createRawSprite())
	, lose_splash(game_data->getRenderer()->createRawSprite())
{

	hasWon = isWinner;

	if(!dim_background->loadTexture("../../Resources/Textures/UI/Dim Background.png"))
	{
		throw;
	}

	if(!win_splash->loadTexture("../../Resources/Textures/UI/win_splash.png"))
	{
		throw;
	}

	if(!lose_splash->loadTexture("../../Resources/Textures/UI/lose_splash.png"))
	{
		throw;
	}

	win_splash->xPos(240.0f);
	win_splash->yPos(180.0f);

	lose_splash->xPos(240.0f);
	lose_splash->yPos(180.0f);

	const float c[3] = { 0.817f, 0.668f, 0.515f };
	menu.addButton((game_data->getWindowWidth() * 0.5f) - 120.0f, game_data->getWindowHeight() * 0.75f - 60.0f, "Return To Main Menu", c, ASGE::COLOURS::ORANGE, 240.0f, 40.0f, "UI/lobbyButton");

	menu.getButton(0).on_click.connect([game_data]()
	{
		game_data->getStateManager()->pop();
		game_data->getStateManager()->pop();
	});
}

void StateGameOver::update(const ASGE::GameTime &)
{
	menu.update();
}

void StateGameOver::render() const
{
	menu.render(Z_ORDER_LAYER::OVERLAY_TEXT);
	game_data->getRenderer()->renderSprite(*dim_background, Z_ORDER_LAYER::OVERLAY_TEXT + 5000);
	if(hasWon)
	{
		game_data->getRenderer()->renderSprite(*win_splash, Z_ORDER_LAYER::OVERLAY + 5000);
	}
	else
	{
		game_data->getRenderer()->renderSprite(*lose_splash, Z_ORDER_LAYER::OVERLAY + 5000);
	}
}

void StateGameOver::onActive()
{
}

void StateGameOver::onInactive()
{
}
