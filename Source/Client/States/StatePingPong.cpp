#include "StatePingPong.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"

StatePingPong::StatePingPong(GameData* game_data)
	: State(game_data)
	, menu(game_data)
{
	menu.addButton(500, 300, "SERVER", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);
	menu.addButton(500, 400, "CLIENT", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);

	menu.getButton(0).on_click.connect([game_data]()
	{
		game_data->getNetworkManager()->initialize(true);
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getNetworkManager()->initialize(false);
	});
}

StatePingPong::~StatePingPong()
{
}

void StatePingPong::update(const ASGE::GameTime& gt)
{
	menu.update();
}

void StatePingPong::render() const
{
	auto renderer = game_data->getRenderer();
	auto network = game_data->getNetworkManager()->network.get();

	if (network)
	{
		if (network->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}

		if (network->isServer())
		{
			renderer->renderText("SERVER", 250, 50, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("CLIENT", 250, 50, ASGE::COLOURS::WHITE);
		}
	}
	else
	{
		menu.render();
	}
}

void StatePingPong::onActive()
{
}

void StatePingPong::onInactive()
{
}