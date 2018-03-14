#include "Paddle.hpp"

//SELF
#include "../../../Architecture/GameData.hpp"

Paddle::Paddle(GameData* game_data)
	: Entity(game_data)
	, sprite(game_data->getRenderer())
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

void Paddle::update(float dt)
{
	sprite.update(dt);

	if (isOwner())
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

void Paddle::render(ASGE::Renderer* renderer) const
{
	renderer->renderSprite(*sprite.getCurrentFrameSprite());
}

void Paddle::receivedPacket(Packet&& p)
{
	p >> sprite.xPos >> sprite.yPos;
}
