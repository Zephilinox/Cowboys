#pragma once
#include <cmath>
#include "TerrainTile.h"
#include "../Architecture/GameData.hpp"
#include "../Architecture/Constants.hpp"

//LIB
#include <jsoncons/json.hpp>

constexpr int mapWidth = 50;
constexpr int mapHeight = 50;

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

	TerrainTile* getTile(int arrayPosX, int arrayPosY);

	//CHRIS - SPAWN IN VALS, USE ARRAY POSES IN JSON
	float getTileXPosAtArrayPos(int x, int y) const;
	float getTileYPosAtArrayPos(int x, int y) const;

	int getJsonXPos(uint32_t owner, uint32_t netID);
	int getJsonYPos(uint32_t owner, uint32_t netID);

	std::vector<MoveData>& getPathToGoal() { return pathToGoal; }
	void clearMoveData();

	void applyOffset(float x, float y);
	void addBuildingToMap(Building& building);

	void loadJSONBuildings(int seed);

	bool findPathFromTo(TerrainTile * startTile, TerrainTile * endTile);
	TerrainTile map[mapWidth][mapHeight];

private:
	bool withinView(ASGE::Sprite* sprite) const;

	float getManhattanDistance(TerrainTile * startNode, TerrainTile * endNode);

	//todo: use std::array so we can pass in runtime values
	char grid[mapWidth][mapHeight];

	GameData* game_data = nullptr;

	//todo: use std::array so we can pass in runtime values

	int seed_used = 0;
	int posJSONCounter = 0;

	float offset_x;
	float offset_y;				

	std::vector<Building> buildings;

	Rng rng_generator;

	//Pathfinding
	std::vector<TerrainTile*> openList;
	std::vector<TerrainTile*> closedList;
	std::vector<MoveData> pathToGoal;
	std::vector<TerrainTile*> neighbours;
	TerrainTile* currentPathingNode;
};