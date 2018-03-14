#include "StatePingPong.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"

StatePingPong::StatePingPong(GameData* game_data)
	: State(game_data)
	, menu(game_data)
{
	if (game_data->getNetworkManager()->network->isServer())
	{
		createEntity<Paddle>(game_data);
		createEntity<Ball>(game_data);
		serverPaddle = static_cast<Paddle*>(getEntity(1));
		serverBall = static_cast<Ball*>(getEntity(2));

		managed_slot_1 = game_data->getNetworkManager()->packet_received.connect([&, game_data](Packet p)
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
						game_data->getNetworkManager()->network->sendPacket(p.senderID, 0, &p);
					}
				} break;
				case hash("Disconnected"):
				{
					std::experimental::erase_if(entities, [p](const auto& entity)
					{
						return entity->entity_info.ownerID == p.senderID;
					});
				} break;
			}
		});

		managed_slot_2 = game_data->getNetworkManager()->packet_received.connect([&, game_data](Packet p)
		{
			switch (p.getID())
			{
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
						ent->receivePacket(std::move(p));
					}
				} break;
				case hash("CreateEntity"):
				{
					EntityInfo info;
					p >> info;
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
				} break;
			}
		});
	}
	else
	{
		createEntity<Paddle>(game_data);
		managed_slot_1 = game_data->getNetworkManager()->packet_received.connect([&, game_data](Packet p)
		{
			switch (p.getID())
			{
				case hash("Entity"):
				{
					EntityInfo info;
					p >> info;
					Entity* ent = getEntity(info.networkID);
					if (ent)
					{
						ent->receivePacket(std::move(p));
					}
				} break;
				case hash("CreateEntity"):
				{
					EntityInfo info;
					p >> info;

					switch (info.type)
					{
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
				} break;
			}
		});
	}
}

StatePingPong::~StatePingPong()
{
	if (game_data->getNetworkManager()->network)
	{
		game_data->getNetworkManager()->network->deinitialize();
		game_data->getNetworkManager()->network.reset(nullptr);
	}
}

void StatePingPong::update(const ASGE::GameTime& gt)
{
	auto network = game_data->getNetworkManager()->network.get();
	const float dt = gt.delta_time.count() / 1000.0f;

	for (auto& ent : entities)
	{
		ent->update(dt);
	}

	if (game_data->getInputManager()->isActionPressed("escape"))
	{
		game_data->getStateManager()->pop();
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

void StatePingPong::render() const
{
	auto renderer = game_data->getRenderer();
	auto network = game_data->getNetworkManager()->network.get();

	for (const auto& ent : entities)
	{
		ent->render(game_data->getRenderer());
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
