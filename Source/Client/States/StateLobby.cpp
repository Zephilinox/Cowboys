#include "StateLobby.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"
#include "StatePingPong.hpp"

StateLobby::StateLobby(GameData* game_data)
	: State(game_data)
	, menu(game_data)
	, lobby(game_data)
{
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 - 40, "SERVER", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2, "CLIENT", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);

	menu.getButton(0).on_click.connect([&, game_data]()
	{
		game_data->getNetworkManager()->initialize(true);
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getNetworkManager()->initialize(false);

		game_data->getNetworkManager()->packet_received.connect([game_data](Packet p)
		{
			if (p.getID() == hash("GameStart"))
			{
				game_data->getStateManager()->push<StatePingPong>();
			}
		});
	});
}

void StateLobby::update(const ASGE::GameTime& /*unused*/)
{
	if (!ready &&
		game_data->getNetworkManager()->network &&
		game_data->getNetworkManager()->network->isServer() &&
		game_data->getNetworkManager()->network->isConnected())
	{
		ready = true;
		lobby.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 - 40, "START", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
		lobby.getButton(0).on_click.connect([&]()
		{
			game_data->getStateManager()->push<StatePingPong>();
			Packet p;
			p.setID(hash("GameStart"));
			game_data->getNetworkManager()->network->sendPacket(0, &p);
		});
	}

	if (ready &&
		(!game_data->getNetworkManager()->network ||
		(game_data->getNetworkManager()->network->isServer() &&
			!game_data->getNetworkManager()->network->isConnected())))
	{
		ready = false;
		lobby.reset();
	}

	if (game_data->getNetworkManager()->network)
	{
		lobby.update();
	}
	else
	{
		menu.update();
	}
}

void StateLobby::render() const
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

		lobby.render();
	}
	else
	{
		menu.render();
	}
}

void StateLobby::onActive()
{
}

void StateLobby::onInactive()
{
}