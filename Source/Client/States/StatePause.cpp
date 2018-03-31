#include "StatePause.hpp"

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/Timer.hpp"
#include "../../Architecture/Constants.hpp"

StatePause::StatePause(GameData* game_data)
	: State(game_data, true, true)
	, menu(game_data)
	, dim_background(game_data->getRenderer()->createRawSprite())
	, pause_image(game_data->getRenderer()->createRawSprite())
{
	if (!dim_background->loadTexture("../../Resources/Textures/UI/Dim Background.png"))
	{
		throw;
	}

	if (!pause_image->loadTexture("../../Resources/Textures/UI/Pause.png"))
	{
		throw;
	}

	const float c[3] = { 0.817f, 0.668f, 0.515f };
	menu.addButton(game_data->getWindowWidth() / 2.0f - 40.0f, game_data->getWindowHeight() / 2.0f - 60.0f, "CONTINUE", c, ASGE::COLOURS::ANTIQUEWHITE, 100.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 60.0f, game_data->getWindowHeight() / 2.0f + 20.0f, "TOGGLE AUDIO", c, ASGE::COLOURS::ANTIQUEWHITE, 140.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 90.0f, game_data->getWindowHeight() / 2.0f + 60.0f, "TOGGLE FULLSCREEN", c, ASGE::COLOURS::ANTIQUEWHITE, 200.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 20.0f, game_data->getWindowHeight() / 2.0f + 100.0f, "EXIT", c, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	menu.getButton(0).on_click.connect([gd = game_data]()
	{
		gd->getStateManager()->pop();
	});

	menu.getButton(1).on_click.connect([this, game_data]()
	{
		game_data->getMusicPlayer()->toggleMuted();
	});

	menu.getButton(2).on_click.connect([game_data]()
	{
		if(game_data->getRenderer()->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
		{
			game_data->getRenderer()->setWindowedMode(ASGE::Renderer::WindowMode::FULLSCREEN);
		}
		else
		{
			game_data->getRenderer()->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
		}
	});

	menu.getButton(3).on_click.connect([game_data]()
	{
		game_data->exit = true;
	});
}

void StatePause::update(const ASGE::GameTime &)
{
	menu.update();

	if(game_data->getInputManager()->isActionPressed(hash("Escape")))
	{
		game_data->getStateManager()->pop();
	}
}

void StatePause::render() const
{
	menu.render(Z_ORDER_LAYER::OVERLAY_TEXT);
	game_data->getRenderer()->renderSprite(*dim_background, Z_ORDER_LAYER::OVERLAY);
	game_data->getRenderer()->renderSprite(*pause_image, Z_ORDER_LAYER::OVERLAY + 100);
}

void StatePause::onActive()
{
}

void StatePause::onInactive()
{
}
