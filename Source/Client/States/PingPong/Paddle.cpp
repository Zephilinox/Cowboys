#include "Paddle.hpp"

//STD
#include <iostream>

//SELF
#include "../../../Architecture/GameData.hpp"

Paddle::Paddle(GameData* game_data)
	: Entity(game_data)
	, sprite(game_data->getRenderer())
{
	entity_info.type = hash("Paddle");
	sprite.addFrame("ppl1", 1);

	if (game_data->getNetworkManager()->client->getID() == 1)
	{
		sprite.xPos = 60;
		sprite.yPos = 720 / 2;
	}
	else
	{
		sprite.xPos = 1100;
		sprite.yPos = 720 / 2;
	}

	mc = game_data->getNetworkManager()->on_network_tick.connect([this]()
	{
		if (isOwner())
		{
			//std::cout << "Paddle Packet Sent\n";
			sendPacket();
		}
	});
}

void Paddle::update(float dt)
{
	sprite.update(dt);

	if (isOwner())
	{
		if (game_data->getInputManager()->isActionDown(hash("up")))
		{
			sprite.yPos -= 1000 * dt;
		}
		else if (game_data->getInputManager()->isActionDown(hash("down")))
		{
			sprite.yPos += 1000 * dt;
		}
	}
}

void Paddle::render(ASGE::Renderer* renderer) const
{
	renderer->renderSprite(*sprite.getCurrentFrameSprite());
}

void Paddle::serialize(Packet& p)
{
	p << sprite.xPos << sprite.yPos;
}

void Paddle::deserialize(Packet& p)
{
	p >> sprite.xPos >> sprite.yPos;
}