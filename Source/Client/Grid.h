#pragma once
#include <cmath>
#include "TerrainTile.h"
#include "../Architecture/GameData.hpp"

//LIB
#include <jsoncons/json.hpp>

constexpr int mapWidth = 20;
constexpr int mapHeight = 12;

class Grid
{
public:

	Grid(GameData* data);
	void randomiseCharGrid(int seed);
	~Grid() = default;
	float GetF();

	void render() const;

	void loadFromJSON(int map_to_load);

	void loadHardCodedMap();

private:
	int xCoord;
	int yCoord;

//	int id;

	float G;
	float H;


	char grid[mapWidth][mapHeight] = {
	{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'w', 'w', 'w', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'r', 'g', 'g', 'g', 'g', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'r', 'g', 'g', 'g', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'w', 'w', 'w'},
	{'w', 'g', 'g', 'r', 'g', 'r', 'g', 'g', 'g', 'w'},
	{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
	};

	GameData* game_data = nullptr;

	TerrainTile map[mapWidth][mapHeight];
};