#include "StateLobby.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"

#include "StateGame.hpp"

StateLobby::StateLobby(GameData* game_data)
	: State(game_data, true, true)
	, menu(game_data)
	, lobby(game_data, false)
	, panel1(game_data, 0.0f)
	, panel2(game_data, 256.0f)
	, panel3(game_data, 512.0f)
	, panel4(game_data, 768.0f)
	, panel5(game_data, 1024.0f)
{
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f - 40.0f, "SERVER", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 70.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f, "CLIENT", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 70.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f + 40.0f, "BACK", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 70.0f, 20.0f);

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
			this->game_data->getStateManager()->push<StateGame>
				(panel1.getSelectedUnitID(),
					panel2.getSelectedUnitID(),
					panel3.getSelectedUnitID(),
					panel4.getSelectedUnitID(),
					panel5.getSelectedUnitID());
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

	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f - 80.0f, game_data->getWindowHeight() * 0.8f, "START", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() * 0.8f, "TOGGLE READY", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 140.0f, 20.0f);
	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f + 180.0f, game_data->getWindowHeight() * 0.8f, "BACK", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	
	lobby.getButton(0).on_click.connect([this]()
	{
		if (this->game_data->getNetworkManager()->server)
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

	//warband selection

	lobby.addButton(0.0f + 25, 440, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 90.0f, 20.0f);
	lobby.addButton(0.0f + 175, 440, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	lobby.addButton(256.0f + 25, 440, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 90.0f, 20.0f);
	lobby.addButton(256.0f + 175, 440, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	lobby.addButton(512.0f + 25, 440, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 90.0f, 20.0f);
	lobby.addButton(512.0f + 175, 440, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	lobby.addButton(768.0f + 25, 440, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 90.0f, 20.0f);
	lobby.addButton(768.0f + 175, 440, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	lobby.addButton(1024.0f + 25, 440, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 90.0f, 20.0f);
	lobby.addButton(1024.0f + 175, 440, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);

	lobby.getButton(3).on_click.connect([this]()
	{
		panel1.decrementSelectedUnit();
	});

	lobby.getButton(4).on_click.connect([this]()
	{
		panel1.incrementSelectedUnit();

	});

	lobby.getButton(5).on_click.connect([this]()
	{
		panel2.decrementSelectedUnit();
	});

	lobby.getButton(6).on_click.connect([this]()
	{
		panel2.incrementSelectedUnit();
	});

	lobby.getButton(7).on_click.connect([this]()
	{
		panel3.decrementSelectedUnit();
	});

	lobby.getButton(8).on_click.connect([this]()
	{
		panel3.incrementSelectedUnit();
	});

	lobby.getButton(9).on_click.connect([this]()
	{
		panel4.decrementSelectedUnit();
	});

	lobby.getButton(10).on_click.connect([this]()
	{
		panel4.incrementSelectedUnit();

	});

	lobby.getButton(11).on_click.connect([this]()
	{
		panel5.decrementSelectedUnit();
	});

	lobby.getButton(12).on_click.connect([this]()
	{
		panel5.incrementSelectedUnit();
	});
}

void StateLobby::update(const ASGE::GameTime&)
{
	auto client = game_data->getNetworkManager()->client.get();
	
	if (client && client->isConnecting())
	{
		lobby.update();
		panel1.update();
		panel2.update();
		panel3.update();
		panel4.update();
		panel5.update();
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

	if (client && client->isConnecting())
	{
		panel1.render(game_data->getRenderer());
		panel2.render(game_data->getRenderer());
		panel3.render(game_data->getRenderer());
		panel4.render(game_data->getRenderer());
		panel5.render(game_data->getRenderer());
	}

	if (server)
	{
		if (server->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}

		renderer->renderText("SERVER", 250, 50, 1.0f, ASGE::COLOURS::BLACK, 10000);
	}
	else if (client)
	{
		if (client->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else if (client->isConnecting())
		{
			renderer->renderText("CONNECTING", 250, 100, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}

		renderer->renderText("CLIENT", 250, 50, 1.0f, ASGE::COLOURS::BLACK, 10000);
	}

	if (client && client->isConnecting())
	{
		renderer->renderText(ready ? "READY" : "NOT READY", 250, 150, 1.0f, ASGE::COLOURS::BLACK, 10000);
		renderer->renderText(other_ready ? "OTHER PLAYER READY" : "OTHER PLAYER NOT READY", 250, 200, 1.0f, ASGE::COLOURS::BLACK, 10000);
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