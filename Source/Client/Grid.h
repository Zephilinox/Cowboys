#pragma once
#include <cmath>
#include "TerrainTile.h"
#include "../Architecture/GameData.hpp"

//LIB
#include <jsoncons/json.hpp>

constexpr int mapWidth = 96;
constexpr int mapHeight = 54;

class Grid
{
public:

	Grid(GameData* data);
	~Grid() = default;

	void render() const;

	void loadHardCodedMap();
	void generateCharGrid(int seed);

	float getTileXPosAtArrayPos(int x, int y);
	float getTileYPosAtArrayPos(int x, int y);

	void applyOffset(float x, float y);

private:

	char grid[mapWidth][mapHeight];

	GameData* game_data = nullptr;

	TerrainTile map[mapWidth][mapHeight];
};