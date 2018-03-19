#include "Warband.h"
#include "Character.h"

Warband::Warband(GameData * game_data)
{
	//Init the characters vector
	units.reserve(number_of_characters);
	for(unsigned int i = 0; i < number_of_characters; i++)
	{
		units.push_back(std::make_unique<Character>(game_data->getRenderer()));
	}
}


void Warband::update(float dt)
{
	for(auto& current_character : units)
	{
		if(current_character->getIsAlive())
		{
			current_character->update(dt);
		}
	}
}

void Warband::render() const
{
	for(const auto& current_character : units)
	{
		current_character->render(game_data->getRenderer());
	}
}

void Warband::turnEnded(int unit_no)
{
	units[unit_no]->turnEnded();
}

void Warband::roundEnded()
{
	for(auto& current_character : units)
	{
		current_character->roundEnded();
	}
}

void Warband::selectUnit()
{
}
