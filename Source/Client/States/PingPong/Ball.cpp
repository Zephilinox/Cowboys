#include "Ball.hpp"

//SELF
#include "../../../Architecture/GameData.hpp"

Ball::Ball(GameData* game_data)
	: Entity(game_data)
	, sprite(game_data->getRenderer())
{
	entity_info.type = hash("Ball");
	sprite.addFrame("DialogueMarker", 1, 0, 0, 2, 2);
	sprite.xPos = 1280 / 2;
	sprite.yPos = 720 / 2;

	mc = game_data->getNetworkManager()->on_network_tick.connect([this]()
	{
		if (isOwner())
		{
			//std::cout << "Ball Packet Sent\n";
			sendPacket();
		}
	});
}

void Ball::update(float dt)
{
	sprite.update(dt);

	if (isOwner())
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
}

void Ball::render(ASGE::Renderer* renderer) const
{
	renderer->renderSprite(*sprite.getCurrentFrameSprite());
}

void Ball::serialize(Packet & p)
{
	p << sprite.xPos << sprite.yPos;
}

void Ball::deserialize(Packet & p)
{
	p >> sprite.xPos >> sprite.yPos;
}
