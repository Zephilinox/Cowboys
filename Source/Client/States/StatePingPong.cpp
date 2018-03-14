#include "StatePingPong.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Entity.hpp"

class Paddle : public Entity
{
public:
	Paddle(GameData* game_data)
		: Entity(game_data)
		, sprite(game_data->getRenderer())
		, game_data(game_data)
	{
		entity_info.type = hash("Paddle");
		sprite.addFrame("ppl1", 1);

		if (game_data->getNetworkManager()->network->isServer())
		{
			sprite.xPos = 60;
			sprite.yPos = 720 / 2;
		}
		else
		{
			sprite.xPos = 1100;
			sprite.yPos = 720 / 2;
		}
	}

	void update(float dt) override final
	{
		sprite.update(dt);

		if (entity_info.ownerID == game_data->getNetworkManager()->network->getID())
		{
			if (game_data->getInputManager()->isActionDown("up"))
			{
				sprite.yPos -= 1000 * dt;
			}
			else if (game_data->getInputManager()->isActionDown("down"))
			{
				sprite.yPos += 1000 * dt;
			}

			Packet p;
			p.setID(hash("Entity"));
			p << &entity_info
				<< sprite.xPos
				<< sprite.yPos;
			game_data->getNetworkManager()->network->sendPacket(0, &p);
		}
	}

	void render(ASGE::Renderer* renderer) const override final
	{
		renderer->renderSprite(*sprite.getCurrentFrameSprite());
	}

	void receivedPacket(uint32_t channel_id, Packet* p) override final
	{
		*p >> sprite.xPos >> sprite.yPos;
	}

	AnimatedSprite sprite;
	GameData* game_data;
};

class Ball : public Entity
{
public:
	Ball(GameData* game_data)
		: Entity(game_data)
		, sprite(game_data->getRenderer())
		, game_data(game_data)
	{
		entity_info.type = hash("Ball");
		sprite.addFrame("DialogueMarker", 1, 0, 0, 2, 2);
		sprite.xPos = 1280 / 2;
		sprite.yPos = 720 / 2;
	}

	void update(float dt) override final
	{
		sprite.update(dt);

		if (game_data->getNetworkManager()->network->isServer())
		{
			if (sprite.yPos < 0)
			{
				dirY = -dirY;
				sprite.yPos = 0;
			}

			if (sprite.yPos + sprite.getCurrentFrameSprite()->height() > game_data->getWindowHeight())
			{
				dirY = -dirY;
				sprite.yPos = game_data->getWindowHeight() - sprite.getCurrentFrameSprite()->height();
			}

			if (sprite.xPos + sprite.getCurrentFrameSprite()->width() < 0 ||
				sprite.xPos > game_data->getWindowWidth())
			{
				sprite.xPos = 1280 / 2;
				dirY = 0;
				movingLeft = game_data->getRandomNumberGenerator()->getRandomInt(0, 1);
			}

			sprite.xPos += 200 * dt * (movingLeft ? -1 : 1);
			sprite.yPos += 200 * dt * dirY;
		}

		if (entity_info.ownerID == game_data->getNetworkManager()->network->getID())
		{
			Packet p;
			p.setID(hash("Entity"));
			p << &entity_info
				<< sprite.xPos
				<< sprite.yPos;
			game_data->getNetworkManager()->network->sendPacket(0, &p);
		}
	}

	void render(ASGE::Renderer* renderer) const override final
	{
		renderer->renderSprite(*sprite.getCurrentFrameSprite());
	}

	void receivedPacket(uint32_t channelID, Packet* p) override final
	{
		*p >> sprite.xPos >> sprite.yPos;
	}

	AnimatedSprite sprite;
	GameData* game_data;
	bool movingLeft = false;
	float dirY = 0;
};

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
						ent->receivedPacket(0, &p);
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
