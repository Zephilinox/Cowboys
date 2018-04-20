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
	lobby.enableKeyboardInput(false);

	callback = game_data->getInput()->addCallbackFnc(ASGE::EventType::E_KEY, &StateLobby::keyHandler, this);

	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f - 40.0f, "SERVER", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 70.0f, 20.0f, "UI/lobbyButton");
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f, "CLIENT", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 70.0f, 20.0f, "UI/lobbyButton");
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f + 40.0f, "BACK", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 70.0f, 20.0f, "UI/lobbyButton");

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
		
		if (p.getID() == hash("ChatMessage"))
		{
			this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
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

		if (p.getID() == hash("ChatMessage"))
		{
			std::string string;
			p >> string;
			chatlog.push_back(string);

			while (chatlog.size() > 7)
			{
				chatlog.pop_front();
			}
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

	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f - 80.0f, game_data->getWindowHeight() * 0.8f, "START", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() * 0.8f, "TOGGLE READY", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 140.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(game_data->getWindowWidth() / 2.0f - 80.0f + 180.0f, game_data->getWindowHeight() * 0.8f, "BACK", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");
	
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

	//warband selection
	float height = 460;
	lobby.addButton(0.0f + 25, height, "Previous", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 90.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(0.0f + 175, height, "Next", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");

	lobby.addButton(256.0f + 25, height, "Previous", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 90.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(256.0f + 175, height, "Next", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");

	lobby.addButton(512.0f + 25, height, "Previous", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 90.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(512.0f + 175, height, "Next", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");

	lobby.addButton(768.0f + 25, height, "Previous", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 90.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(768.0f + 175, height, "Next", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");

	lobby.addButton(1024.0f + 25, height, "Previous", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 90.0f, 20.0f, "UI/lobbyButton");
	lobby.addButton(1024.0f + 175, height, "Next", ASGE::COLOURS::FLORALWHITE, ASGE::COLOURS::ORANGE, 60.0f, 20.0f, "UI/lobbyButton");

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
		
		if (game_data->getInputManager()->isKeyPressed(ASGE::KEYS::KEY_ENTER))
		{
			std::string username("Client " + std::to_string(game_data->getNetworkManager()->client->getID()) + std::string(": "));
			Packet p;
			p.setID(hash("ChatMessage"));
			p << std::string(username + input);
			this->game_data->getNetworkManager()->client->sendPacket(0, &p);
			std::cout << std::string(username + input) << "\n";
			input.clear();
		}
	}
	else
	{
		menu.update();
	}

	//uhoh, windows specific shit, sorry
	capslockOn = (unsigned short(GetKeyState(0x14)) & 0xffff) != 0;
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
		renderer->renderText("SERVER", 50, 550, 1.0f, ASGE::COLOURS::BLACK, 10000);

		if (server->isConnected())
		{
			renderer->renderText("CONNECTED", 50, 580, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 50, 580, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
	}
	else if (client)
	{
		renderer->renderText("CLIENT", 50, 550, 1.0f, ASGE::COLOURS::BLACK, 10000);

		if (client->isConnected())
		{
			renderer->renderText("CONNECTED", 50, 580, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else if (client->isConnecting())
		{
			renderer->renderText("CONNECTING", 50, 580, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 50, 580, 1.0f, ASGE::COLOURS::BLACK, 10000);
		}
	}

	if (client && client->isConnecting())
	{
		renderer->renderText(ready ? "READY" : "NOT READY", 50, 610, 1.0f, ASGE::COLOURS::BLACK, 10000);
		renderer->renderText(other_ready ? "OTHER PLAYER READY" : "OTHER PLAYER NOT READY", 50, 640, 1.0f, ASGE::COLOURS::BLACK, 10000);

		const float c[3] = { 0.2f, 0.2f, 0.2f };
		auto y = 550;
		for (const auto& msg : chatlog)
		{
			renderer->renderText(msg, 850, y, 1.0f, c, 10000);
			y += 20;
		}

		renderer->renderText(std::string("Chat Message: ") + input, 850, 690, 1.0f, ASGE::COLOURS::BLACK, 10000);

		lobby.render();
	}
	else
	{
		menu.render();
	}
}

void StateLobby::keyHandler(const ASGE::SharedEventData data)
{
	static std::vector<int> valid_keys = {};

	const ASGE::KeyEvent* key_event = static_cast<const ASGE::KeyEvent*>(data.get());
	auto action = key_event->action;
	auto key = key_event->key;
	auto mods = key_event->mods;

	if (action == ASGE::KEYS::KEY_PRESSED)
	{
		if (key == ASGE::KEYS::KEY_DELETE ||
			key == ASGE::KEYS::KEY_BACKSPACE &&
			input.size())
		{
			input.pop_back();
		}
		else if (key >= ASGE::KEYS::KEY_SPACE && key <= ASGE::KEYS::KEY_RIGHT_BRACKET)
		{
			bool shift = mods & 0x0001;
			bool capitals = shift;
			capitals = capslockOn ? !capitals : capitals;

			if (!capitals)
			{
				key = tolower(char(key));
			}

			if (shift)
			{
				switch (key)
				{
				case '1':
					key = '!';
					break;
				case '2':
					key = '"';
					break;
				case '3':
					key = '�';
					break;
				case '4':
					key = '$';
					break;
				case '5':
					key = '%';
					break;
				case '6':
					key = '^';
					break;
				case '7':
					key = '&';
					break;
				};
			}

			input.push_back(key);
		}
	}
}

void StateLobby::onActive()
{
}

void StateLobby::onInactive()
{
}