#pragma once
#include <cmath>
#include "TerrainTile.h"
#include "../Architecture/GameData.hpp"

//LIB
#include <jsoncons/json.hpp>

constexpr int mapWidth = 31;
constexpr int mapHeight = 17;

class Grid
{
public:

	Grid(GameData* data);
	~Grid() = default;
	float GetF();

	void render() const;

	void loadHardCodedMap();
	void generateCharGrid(int seed);

private:
	int xCoord;
	int yCoord;

//	int id;

	float G;
	float H;


	char grid[mapWidth][mapHeight];

	GameData* game_data = nullptr;

	TerrainTile map[mapWidth][mapHeight];
};