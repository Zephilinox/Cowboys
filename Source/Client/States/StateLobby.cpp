#include "StateLobby.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"

#include "StatePingPong.hpp"

StateLobby::StateLobby(GameData* game_data)
	: State(game_data)
	, menu(game_data)
	, lobby(game_data)
{
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 - 40, "SERVER", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2, "CLIENT", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 + 40, "BACK", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);

	auto server_lam = [this](Packet p)
	{
		if (p.getID() == hash("ClientReady"))
		{
			this->game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [p](const ClientInfo& info)
			{
				return p.senderID != info.id;
			});
		}

		if (p.getID() == hash("Connected"))
		{
			p.reset();
			p.setID(hash("ClientReady"));
			p << ready;
			this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
		}

		if (p.getID() == hash("Disconnected"))
		{
			other_ready = false;
		}
	};

	auto client_lam = [this](Packet p)
	{
		if (p.getID() == hash("ClientReady"))
		{
			p >> other_ready;
		}

		if (p.getID() == hash("GameStart"))
		{
			this->game_data->getStateManager()->pop();
			this->game_data->getStateManager()->push<StatePingPong>();
		}
		
		if (p.getID() == hash("Connected"))
		{
			p.reset();
			p.setID(hash("ClientReady"));
			p << ready;
			this->game_data->getNetworkManager()->client->sendPacket(0, &p);
		}
	};

	menu.getButton(0).on_click.connect([this, server_lam, client_lam]()
	{
		this->game_data->getNetworkManager()->startHost();
		mc1 = this->game_data->getNetworkManager()->server->on_packet_received.connect(server_lam);
		mc2 = this->game_data->getNetworkManager()->client->on_packet_received.connect(client_lam);
	});

	menu.getButton(1).on_click.connect([this, client_lam]()
	{
		this->game_data->getNetworkManager()->startClient();
		mc2 = this->game_data->getNetworkManager()->client->on_packet_received.connect(client_lam);
	});

	menu.getButton(2).on_click.connect([this]()
	{
		this->game_data->getStateManager()->pop();
	});

	lobby.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 - 40, "START", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	lobby.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2, "TOGGLE READY", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	lobby.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 + 40, "BACK", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	
	lobby.getButton(0).on_click.connect([this]()
	{
		if (this->game_data->getNetworkManager()->server && ready && other_ready)
		{
			Packet p;
			p.setID(hash("GameStart"));
			this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
		}
	});

	lobby.getButton(1).on_click.connect([this]()
	{
		ready = !ready;

		if (this->game_data->getNetworkManager()->client->isConnected())
		{
			Packet p;
			p.setID(hash("ClientReady"));
			p << ready;
			this->game_data->getNetworkManager()->client->sendPacket(0, &p);
		}
	});

	lobby.getButton(2).on_click.connect([this]()
	{
		ready = false;
		other_ready = false;
		this->game_data->getNetworkManager()->stopServer();
		this->game_data->getNetworkManager()->stopClient();
	});
}

void StateLobby::update(const ASGE::GameTime&)
{
	auto client = game_data->getNetworkManager()->client.get();
	
	if (client && client->isConnecting())
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
	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();

	if (server)
	{
		if (server->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}

		renderer->renderText("SERVER", 250, 50, ASGE::COLOURS::WHITE);
	}
	else if (client)
	{
		if (client->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}
		else if (client->isConnecting())
		{
			renderer->renderText("CONNECTING", 250, 100, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}

		renderer->renderText("CLIENT", 250, 50, ASGE::COLOURS::WHITE);
	}

	if (client && client->isConnecting())
	{
		renderer->renderText(ready ? "READY" : "NOT READY", 250, 150, ASGE::COLOURS::WHITE);
		renderer->renderText(other_ready ? "OTHER PLAYER READY" : "OTHER PLAYER NOT READY", 250, 200, ASGE::COLOURS::WHITE);
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