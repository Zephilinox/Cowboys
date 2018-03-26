#include "Warband.h"
#include "Character.h"

Warband::Warband(GameData * game_data)
{
	//Init the characters vector
	units.reserve(number_of_characters);


	units.push_back(std::make_unique<Hero>(game_data->getRenderer() ));
	//Read in stats from hero selection
	
	for(unsigned int i = 1; i < number_of_characters; i++)
	{
		units.push_back(std::make_unique<Character>(game_data->getRenderer() ));
		//Read in stats from character selection here?
	}
}


void Warband::update(float dt)
{
	for(auto& current_unit : units)
	{
		if(current_unit->getIsAlive())
		{
			current_unit->update(dt);
		}
	}
}

void Warband::render() const
{
	for(const auto& current_unit : units)
	{
		current_unit->render(game_data->getRenderer());
	}
}

void Warband::turnEnded(int unit_no)
{
	units[unit_no]->turnEnded();
}

void Warband::roundEnded()
{
	for(auto& current_unit : units)
	{
		current_unit->roundEnded();
	}
}

void Warband::selectUnit()
{
}
