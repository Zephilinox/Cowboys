#pragma once
#include <cmath>
#include "TerrainTile.h"
#include "../Architecture/GameData.hpp"

//LIB
#include <jsoncons/json.hpp>

constexpr int mapWidth = 150;
constexpr int mapHeight = 150;

struct Building
{
	int buildingWidth = 0;
	int buildingHeight = 0;
	int arrayPosX = 0;
	int arrayPosY = 0;
	std::string buildingTextureSource;
};

class Grid
{
public:
	Grid(GameData* data);
	~Grid() = default;

	void render() const;

	void loadHardCodedMap();
	void generateCharGrid(int seed);

	float getTileXPosAtArrayPos(int x, int y) const;
	float getTileYPosAtArrayPos(int x, int y) const;

	void applyOffset(float x, float y);
	void addBuildingToMap(Building & building);

	void loadJSONBuildings(int seed);

private:
	bool withinView(ASGE::Sprite* sprite) const;

	char grid[mapWidth][mapHeight];

	GameData* game_data = nullptr;

	TerrainTile map[mapWidth][mapHeight];

	float offset_x;
	float offset_y;				

	std::vector<Building> buildings;
};