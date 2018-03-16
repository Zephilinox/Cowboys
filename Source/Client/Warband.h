#pragma once

#include "Character.h"
#include "..\Architecture\GameData.hpp"

constexpr unsigned int number_of_characters = 4;

class Warband
{
public:
	Warband(GameData* game_data);
	~Warband() = default;

	Character* getCharacter(int unit_no);

	void update(float dt);
	void render() const;
	void turnEnded(int unit_no);
	void roundEnded();

	//Ideas functions for gameplay

	//after selecting unit with mouse?
	void selectUnit(); // Or Set Controlled unit?

private:
	GameData* game_data;
	std::vector <Character> units;


};