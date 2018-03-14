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

	menu.getButton(0).on_click.connect([this, game_data]()
	{
		game_data->getNetworkManager()->initialize(true);

		createEntity<Paddle>(this->game_data);
		createEntity<Ball>(this->game_data);
		serverPaddle = static_cast<Paddle*>(getEntity(1));
		serverBall = static_cast<Ball*>(getEntity(2));

	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getNetworkManager()->initialize(false);
	});

	game_data->getNetworkManager()->packet_received.connect([&, game_data](Packet p)
	{
		switch (p.getID())
		{
			case hash("Connected"):
			{
				if (game_data->getNetworkManager()->network->isServer())
				{
					for (const auto& ent : entities)
					{
						p.reset();
						p.setID(hash("CreateEntity"));
						p << &ent->entity_info;
						game_data->getNetworkManager()->network->sendPacket(p.senderID, 0, &p);
					}
				}
			} break;
			case hash("Disconnected"):
			{
				if (game_data->getNetworkManager()->network->isServer())
				{
					std::experimental::erase_if(entities, [p](const auto& entity)
					{
						return entity->entity_info.ownerID == p.senderID;
					});
				}
			} break;
		}
	});

	game_data->getNetworkManager()->packet_received.connect([&, game_data](Packet p)
	{
		switch (p.getID())
		{
			case hash("Entity"):
			{
				EntityInfo info;
				p >> &info;
				Entity* ent = getEntity(info.networkID);
				if (ent && //exists
					ent->entity_info.ownerID == info.ownerID && //owners match
					ent->entity_info.type == info.type) //types match
				{
					if (p.senderID == 1 || //client received packet, we trust the server
						info.ownerID == p.senderID) //received packet from client, make sure they aren't lying about what they own
					{
						ent->receivedPacket(std::move(p));
					}
				}
			} break;
			case hash("CreateEntity"):
			{
				//so if client sends a spawn ent, just trust it. create the ent with the owner id and entity id it gave us. but then the network ID might not be right, so.. we send back the network id?
				EntityInfo info;
				p >> &info;
				if (game_data->getNetworkManager()->network->isServer()) //server
				{
					switch (info.type)
					{
						case hash("Paddle"):
						{
							createEntity<Paddle>(p.senderID, game_data);
						} break;
						case hash("Ball"):
						{
							createEntity<Ball>(p.senderID, game_data);
						} break;
					}
				}
				else
				{
					switch (info.type)
					{
						//issue with constructor data, oh no
						case hash("Paddle"):
						{
							entities.emplace_back(std::make_unique<Paddle>(game_data));
						} break;
						case hash("Ball"):
						{
							entities.emplace_back(std::make_unique<Ball>(game_data));
						} break;
					}

					entities.back()->entity_info = info;
				}
			} break;
			case hash("ClientID"):
			{
				createEntity<Paddle>(game_data);
			}
		}
	});
}

StatePingPong::~StatePingPong()
{
}

void StatePingPong::update(const ASGE::GameTime& gt)
{
	auto network = game_data->getNetworkManager()->network.get();
	if (network && network->isInitialized())
	{
		const float dt = gt.delta_time.count() / 1000.0f;

		for (auto& ent : entities)
		{
			ent->update(dt);
		}

		if (network->isServer())
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
			for (const auto& ent : entities)
			{
				if (ent->entity_info.ownerID != 1 &&
					ent->entity_info.type == hash("Paddle")) //1 is server id
				{
					clientPaddleID = ent->entity_info.networkID;
					break;
				}
			}
			clientPaddle = static_cast<Paddle*>(getEntity(clientPaddleID));
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
