#include "StatePingPong.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"

StatePingPong::StatePingPong(GameData* game_data)
	: State(game_data)
	, menu(game_data)
{
	menu.addButton(500, 300, "SERVER", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);
	menu.addButton(500, 400, "CLIENT", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);

	auto serverPacketReceive = [this](Packet p)
	{
		switch (p.getID())
		{
		case hash("Connected"):
		{
			for (const auto& ent : entities)
			{
				p.reset();
				p.setID(hash("CreateEntity"));
				p << ent->entity_info;
				this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
			}
		} break;
		case hash("Disconnected"):
		{
			std::experimental::erase_if(entities, [p](const auto& entity)
			{
				return entity->entity_info.ownerID == p.senderID;
			});
		} break;
		case hash("Entity"):
		{
			EntityInfo info;
			p >> info;
			Entity* ent = getEntity(info.networkID);
			if (ent && //exists
				ent->entity_info.ownerID == info.ownerID && //owners match
				ent->entity_info.type == info.type && //types match
				info.ownerID == p.senderID) //client owns it
			{
				this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
			}
		} break;
		case hash("CreateEntity"):
		{
			EntityInfo info;
			p >> info;
			std::cout << "server create ent : " << info.networkID << " " << info.ownerID << "\n";
			switch (info.type)
			{
			case hash("Paddle"):
			{
				createEntity<Paddle>(p.senderID, this->game_data);
			} break;
			case hash("Ball"):
			{
				createEntity<Ball>(p.senderID, this->game_data);
			} break;
			}
		} break;
		}
	};

	auto clientPacketReceive = [this](Packet p)
	{
		switch (p.getID())
		{
		case hash("Connected"):
		{
			createEntity<Paddle>(this->game_data);
		} break;
		case hash("Entity"):
		{
			EntityInfo info;
			p >> info;
			Entity* ent = getEntity(info.networkID);

			if (ent && !ent->isOwner())
			{
				ent->receivePacket(Packet(p));
			}
		} break;
		case hash("CreateEntity"):
		{
			EntityInfo info;
			p >> info;

			std::cout << "client create ent : " << info.networkID << " " << info.ownerID << "\n";
			switch (info.type)
			{
			case hash("Paddle"):
			{
				entities.emplace_back(std::make_unique<Paddle>(this->game_data));
			} break;
			case hash("Ball"):
			{
				entities.emplace_back(std::make_unique<Ball>(this->game_data));
			} break;
			}

			entities.back()->entity_info = info;
		} break;
		}
	};

	menu.getButton(0).on_click.connect([this, serverPacketReceive, clientPacketReceive]()
	{
		entities.clear();
		this->game_data->getNetworkManager()->startHost();

		managed_slot_1 = this->game_data->getNetworkManager()->server->on_packet_received.connect(serverPacketReceive);
		managed_slot_2 = this->game_data->getNetworkManager()->client->on_packet_received.connect(clientPacketReceive);

		createEntity<Paddle>(this->game_data);
		createEntity<Ball>(this->game_data);
		serverPaddle = static_cast<Paddle*>(getEntity(1));
		serverBall = static_cast<Ball*>(getEntity(2));
	});

	menu.getButton(1).on_click.connect([this, clientPacketReceive]()
	{
		entities.clear();
		this->game_data->getNetworkManager()->startClient();

		managed_slot_1 = this->game_data->getNetworkManager()->client->on_packet_received.connect(clientPacketReceive);
	});
}

StatePingPong::~StatePingPong()
{
	game_data->getNetworkManager()->stopHost();
}

void StatePingPong::update(const ASGE::GameTime& gt)
{
	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();
	if (client && client->isConnected())
	{
		const float dt = gt.delta_time.count() / 1000.0f;

		for (auto& ent : entities)
		{
			ent->update(dt);
		}

		if (server)
		{
			//Simple AABB collision check
			if (serverPaddle->sprite.xPos < serverBall->sprite.xPos + serverBall->sprite.getCurrentFrameSprite()->width() &&
				serverPaddle->sprite.xPos + serverPaddle->sprite.getCurrentFrameSprite()->width() > serverBall->sprite.xPos &&
				serverPaddle->sprite.yPos < serverBall->sprite.yPos + serverBall->sprite.getCurrentFrameSprite()->height() &&
				serverPaddle->sprite.yPos + serverPaddle->sprite.getCurrentFrameSprite()->height() > serverBall->sprite.yPos)
			{
				serverBall->movingLeft = false;
				serverBall->dirY = game_data->getRandomNumberGenerator()->getRandomFloat(-0.8, 0.8);
			}

			uint32_t clientPaddleID = 0;
			clientPaddle = nullptr;
			for (const auto& ent : entities)
			{
				if (!ent->isOwner() &&
					ent->entity_info.type == hash("Paddle"))
				{
					clientPaddleID = ent->entity_info.networkID;
					clientPaddle = static_cast<Paddle*>(getEntity(clientPaddleID));
					break;
				}
			}

			if (clientPaddle)
			{
				if (clientPaddle->sprite.xPos < serverBall->sprite.xPos + serverBall->sprite.getCurrentFrameSprite()->width() &&
					clientPaddle->sprite.xPos + clientPaddle->sprite.getCurrentFrameSprite()->width() > serverBall->sprite.xPos &&
					clientPaddle->sprite.yPos < serverBall->sprite.yPos + serverBall->sprite.getCurrentFrameSprite()->height() &&
					clientPaddle->sprite.yPos + clientPaddle->sprite.getCurrentFrameSprite()->height() > serverBall->sprite.yPos)
				{
					serverBall->movingLeft = true;
					serverBall->dirY = game_data->getRandomNumberGenerator()->getRandomFloat(-0.8, 0.8);
				}
			}
		}
	}
	else
	{
		menu.update();
	}

	if (game_data->getInputManager()->isActionPressed("escape"))
	{
		game_data->getStateManager()->pop();
	}
}

void StatePingPong::render() const
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
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}

		renderer->renderText("CLIENT", 250, 50, ASGE::COLOURS::WHITE);
	}

	if (client && client->isConnected())
	{
		for (const auto& ent : entities)
		{
			ent->render(game_data->getRenderer());
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

Entity* StatePingPong::getEntity(uint32_t networkID)
{
	auto it = std::find_if(entities.begin(), entities.end(), [&](const auto& ent)
	{
		return ent->entity_info.networkID == networkID;
	});

	if (it != entities.end())
	{
		return it->get();
	}

	return nullptr;
}
