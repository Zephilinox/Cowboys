#include "Grid.h"

Grid::Grid(GameData* data)
	: game_data(data)
{
}

void Grid::generateCharGrid(int seed)
{
	r_n_generator.setSeed(seed);
	
	//use seed to load JSON into buildings container
	loadJSONBuildings(seed);

	std::vector<std::pair<int, int>> rocks;

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			char new_char = 'g';
			float val = r_n_generator.getRandomFloat(0, 1);

			if (val < 0.02f)
			{
				new_char = 'r';
				rocks.push_back(std::make_pair(x, y));
			}

			grid[x][y] = new_char;
		}
	}

	std::vector<std::pair<int, int>> rocks2;

	for (auto rock_pos : rocks)
	{
		float rand = r_n_generator.getRandomFloat(0, 1);

		if (rand < 0.1f)
		{
			continue;
		}

		auto x = rock_pos.first;
		auto y = rock_pos.second;

		if (x < mapWidth - 1 && y < mapHeight - 1)
		{
			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y] = 'r';
				rocks2.push_back(std::make_pair(x + 1, y));
			}

			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x][y + 1] = 'r';
				rocks2.push_back(std::make_pair(x, y + 1));
			}

			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y + 1] = 'r';
				rocks2.push_back(std::make_pair(x + 1, y + 1));
			}
		}
	}
	
	for (auto rock_pos : rocks2)
	{
		float rand = r_n_generator.getRandomFloat(0, 1);

		if (rand < 0.1f)
		{
			continue;
		}

		auto x = rock_pos.first;
		auto y = rock_pos.second;

		if (x < mapWidth - 1 && y < mapHeight - 1)
		{
			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y] = 'r';
			}

			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x][y + 1] = 'r';
			}

			rand = r_n_generator.getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y + 1] = 'r';
			}
		}
	}
}

TerrainTile* Grid::getTile(int arrayPosX, int arrayPosY)
{
	return &map[arrayPosX][arrayPosY];
}

float Grid::getTileXPosAtArrayPos(int x, int y) const
{
	return map[x][y].getTerrainSprite()->xPos();
}

float Grid::getTileYPosAtArrayPos(int x, int y) const
{
	return map[x][y].getTerrainSprite()->yPos();
}

void Grid::clearMoveData()
{
	pathToGoal.clear();
}

void Grid::applyOffset(float x, float y)
{
	offset_x = x;
	offset_y = y;
}

void Grid::addBuildingToMap(Building& building)
{
	int buildingStartPosX = building.arrayPosX;
	int buildingStartPosY = building.arrayPosY;
	int buildingHeight = building.buildingHeight;
	int buildingWidth = building.buildingWidth;
	std::string texture_source = building.buildingTextureSource;
	
	//block access to all tiles EXCEPT right in front of the building
	//TODO confirm this loop doesn't need to be buildingHeight - 1?
	for(int x = buildingStartPosX; x < buildingStartPosX + buildingWidth; x++)
	{
		for(int y = buildingStartPosY; y < buildingStartPosY + buildingHeight; y++)
		{
			map[x][y].setIsBlocked(true);
		}
	}

	map[buildingStartPosX][buildingStartPosY].getTerrainSprite()->loadTexture(texture_source);
	map[buildingStartPosX][buildingStartPosY].setIsBlocked(true);
}

void Grid::loadJSONBuildings(int seed)
{
	std::ifstream file("../../Resources/mapData.json");
	jsoncons::json layout_JSON_obj;
	file >> layout_JSON_obj;

	seed_used = seed;

	std::string layoutStr = "layout" + std::to_string(seed);

	int buildingsCount = layout_JSON_obj[layoutStr]["numberOfBuildings"].as_int();
	int counter = 1;

	while(counter <= buildingsCount)
	{
		std::string building_no = "building" + std::to_string(counter);

		Building new_build;
		try
		{
			//STATS
			new_build.buildingHeight = layout_JSON_obj[layoutStr][building_no]["height"].as_int();
			new_build.buildingWidth = layout_JSON_obj[layoutStr][building_no]["width"].as_int();
			new_build.arrayPosX = layout_JSON_obj[layoutStr][building_no]["arrayX"].as_int();
			new_build.arrayPosY = layout_JSON_obj[layoutStr][building_no]["arrayY"].as_int();
			new_build.buildingTextureSource = layout_JSON_obj[layoutStr][building_no]["texture"].as_string();
			buildings.push_back(std::move(new_build));
		}
		catch(std::runtime_error& e)
		{
			std::cout << "ERROR INFO: " << e.what() << "\n";
		}

		counter++;
	}
}


void Grid::render() const
{
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			auto* terrain_sprite = map[x][y].getTerrainSprite();
			auto* overlay_sprite = map[x][y].getOverlaySprite();
			if (withinView(terrain_sprite))
			{
				terrain_sprite->xPos(terrain_sprite->xPos() - offset_x);
				terrain_sprite->yPos(terrain_sprite->yPos() - offset_y);
				overlay_sprite->xPos(overlay_sprite->xPos() - offset_x);
				overlay_sprite->yPos(overlay_sprite->yPos() - offset_y);
				map[x][y].render(game_data->getRenderer());
				terrain_sprite->xPos(terrain_sprite->xPos() + offset_x);
				terrain_sprite->yPos(terrain_sprite->yPos() + offset_y);
				overlay_sprite->xPos(overlay_sprite->xPos() + offset_x);
				overlay_sprite->yPos(overlay_sprite->yPos() + offset_y);
			}
		}
	}
}

int Grid::getJsonXPos(uint32_t owner, uint32_t netID)
{

	//TODO convert getJSONXPos into 1 function that returns a pair, which can the be put into the function call to set position
	std::ifstream file("../../Resources/mapData.json");
	jsoncons::json positions_JSON_obj;
	file >> positions_JSON_obj;

	std::string layoutStr = "layout" + std::to_string(seed_used);
	std::string spawnPos = "spawnPositions" + std::to_string(owner);
	std::string posXStr = "positionX" + std::to_string(netID);
	int xPos = -1;

		try
		{
			//STATS
			xPos = positions_JSON_obj[layoutStr][spawnPos][posXStr].as_int();
		}
		catch(std::runtime_error& e)
		{
			std::cout << "ERROR INFO: " << e.what() << "\n";
		}

		return xPos;
}

int Grid::getJsonYPos(uint32_t owner, uint32_t netID)
{
	std::ifstream file("../../Resources/mapData.json");
	jsoncons::json positions_JSON_obj;
	file >> positions_JSON_obj;

	std::string layoutStr = "layout" + std::to_string(seed_used);
	std::string spawnPos = "spawnPositions" + std::to_string(owner);
	std::string posYStr = "positionY" + std::to_string(netID);
	int yPos = -1;

	try
	{
		//STATS
		yPos = positions_JSON_obj[layoutStr][spawnPos][posYStr].as_int();
	}
	catch(std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}

	return yPos;
}

void Grid::loadHardCodedMap()
{
	int new_id = 0;
	for(int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			map[x][y].initialise(grid[x][y], game_data->getRenderer(), x, y);
			map[x][y].getTerrainSprite()->xPos(x * map[x][y].getTerrainSprite()->width());
			map[x][y].getTerrainSprite()->yPos(y * map[x][y].getTerrainSprite()->height());
			map[x][y].getOverlaySprite()->xPos(x * map[x][y].getTerrainSprite()->width());
			map[x][y].getOverlaySprite()->yPos(y * map[x][y].getTerrainSprite()->height());
			map[x][y].id = new_id;
			new_id++;
		}
	}

	for(auto& building : buildings)
	{
		addBuildingToMap(building);
	}
}

bool Grid::withinView(ASGE::Sprite* sprite) const
{
	float x = sprite->xPos() - offset_x;
	float y = sprite->yPos() - offset_y;

	if (x + sprite->width() > 0 &&
		x  < game_data->getWindowWidth() &&
		y + sprite->height() > 0 &&
		y < game_data->getWindowHeight())
	{
		return true;
	}

	return false;
}

float Grid::getManhattanDistance(TerrainTile* startNode, TerrainTile* endNode)
{
	float x = (float)(fabs((float)(startNode->xCoord - endNode->xCoord)));
	float y = (float)(fabs((float)(startNode->yCoord - startNode->yCoord)));

	return x + y;
};

bool Grid::findPathFromTo(TerrainTile* startTile, TerrainTile* endTile)
{

	if(endTile->getIsBlocked() || endTile->getIsOccupied() || endTile == nullptr)
	{
		std::cout << "You can't path to a blocked object\n";
		return false;
	}

	bool pathFound = false;

	startTile->dist_from_start = 0;
	startTile->dist_to_goal = getManhattanDistance(startTile, endTile);
	startTile->parent = nullptr;

	openList.push_back(startTile);

	currentPathingNode = startTile;

	//loop
	//currentPathingNode = node in openList with lowest f_cost
	//copy currentPathingNode to closed lise
	//remove current from open list

	do  // while(pathFound = false)
	{

		float best_cost = 99999.0f;
		int cellIndex = -1;
		for(size_t i = 0; i < openList.size(); i++)
		{
			if(openList[i]->getCombinedCost() < best_cost)
			{
				best_cost = openList[i]->getCombinedCost();
				cellIndex = i;
			}
		}

		if(cellIndex >= 0)
		{
			//Get next cell from open list
			currentPathingNode = openList[cellIndex];
			//copy currentPathingNode to closed list
			closedList.push_back(currentPathingNode);
			//remove current from open list
			//TODO erase remove?
			openList.erase(openList.begin() + cellIndex);
			//openList.erase(std::remove(openList.begin(), openList.end(), cellIndex), openList.end() ) );
		}

		if(currentPathingNode->id == endTile->id)
		{
			std::cout << "path found!";
			//function to put nodes list into gamesState so the player can traverse the list in order
			break;
		}

		//get neighbours of current node

		if(currentPathingNode->xCoord >= 1 && currentPathingNode->yCoord >= 1
			&& currentPathingNode->xCoord <= mapWidth - 2 && currentPathingNode->yCoord <= mapHeight - 2)
		{
			//somewhere in the middle, get all 4 neighbours.
			//Left
			if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
			}
			//Right
			if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
			}
			//Up
			if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
			}
			//Down
			if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
			}
		}

		//on left edge
		else if(currentPathingNode->xCoord <= 0)
		{

			if(currentPathingNode->yCoord <= 0)
			{
				//top left corner, get right and down
				//Right
				if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
				}
				//Down
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
				}

			}
			else if(currentPathingNode->yCoord >= mapHeight - 1)
			{
				//bottom left corner, get right and up
				//Right
				if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
				}
				//Up
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
				}
			}
			else
			{
				//on left edge, get up, right and down
				//Right
				if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
				}
				//Up
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
				}
				//Down
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
				}
			}
		}
		// on right edge
		else if(currentPathingNode->xCoord >= mapWidth - 1)
		{
			if(currentPathingNode->yCoord <= 0)
			{
				//top right corner, get left and down
				//Left
				if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
				}
				//Down
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
				}

			}
			else if(currentPathingNode->yCoord >= mapHeight - 1)
			{
				//bottom right corner, get left and up
				//Left
				if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
				}
				//Up
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
				}
			}
			else
			{
				//on right edge, get up, left and down
				//Left
				if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
				}
				//Up
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
				}
				//Down
				if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
				{
					neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
				}
			}
		}
		else if(currentPathingNode->yCoord <= 0)
		{
			//On top edge, get left, down and right
			//Left
			if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
			}
			//Right
			if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
			}
			//Down
			if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord + 1]);
			}
		}
		else
		{
			//on bottom edge, get left, right and up
			//Left
			if(!map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord - 1][currentPathingNode->yCoord]);
			}
			//Right
			if(!map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsBlocked() || !map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord + 1][currentPathingNode->yCoord]);
			}
			//Up
			if(!map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsBlocked() || !map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1].getIsOccupied())
			{
				neighbours.push_back(&map[currentPathingNode->xCoord][currentPathingNode->yCoord - 1]);
			}
		}


		//for each neighbour of current node
		//	if neighbour isBlocked or neighbour is in closed list
		//		skip neighbour
		for(auto& neighbour : neighbours)
		{
			bool in_closed_list = false;
			bool in_open_list = false;

			//find out if neighbour is in closed list 
			for(auto& elem : closedList)
			{
				if(elem->id == neighbour->id)
				{
					in_closed_list = true;
					break;
				}
			}
			//if not blocked and isn't in closed list
			if(!neighbour->getIsBlocked() && !in_closed_list)
			{
				for(auto& elem : openList)
				{
					if(elem->id == neighbour->id)
					{
						in_open_list = true;
						break;
					}
				}

				//if path to neighbour is shorter OR neighbour is not in open
				if(neighbour->getCombinedCost() < currentPathingNode->getCombinedCost() || !in_open_list)
				{
					//	set f_cost of neighbour
					//	set parent of neighbour to current
					neighbour->setDistanceFromStart(getManhattanDistance(neighbour, startTile));
					neighbour->setDistanceToGoal(getManhattanDistance(neighbour, endTile) );
					neighbour->setParent(currentPathingNode);
					//	if neighbour is not in Open list
					//		add neighbour to open list
					if(!in_open_list)
					{
						openList.push_back(neighbour);
					}
				}
			}

		}

		//clean out neighbours container
		neighbours.clear();
		if(openList.empty())
		{
			std::cout << "no path found";
			closedList.clear();
			openList.clear();
			return false;
		}
	} while(pathFound == false);

	while(currentPathingNode->parent != nullptr)
	{
		MoveData move_data;
		move_data.x = currentPathingNode->getTerrainSprite()->xPos();
		move_data.y = currentPathingNode->getTerrainSprite()->yPos();
		move_data.time_units = currentPathingNode->getMoveDifficultyModifier();

		pathToGoal.push_back(std::move(move_data));

		currentPathingNode->getTerrainSprite()->loadTexture("../../Resources/Textures/Tiles/grassTile.png");
		currentPathingNode = currentPathingNode->parent;
	}
	openList.clear();
	closedList.clear();

	for (int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			map[x][y].parent = nullptr;
		}
	}

	return true;
}

void Grid::getFogOfWar(float view_distance, int start_x, int start_y, int end_x, int end_y)
{

	//void plotLine(int x0, int y0, int x1, int y1)
	int dx = abs(end_x - start_x);
	int sx = start_x < end_x ? 1 : -1;
	int dy = -abs(end_y - start_y);
	int sy = start_y < end_y ? 1 : -1;
	int err = dx + dy;
	int e2; /* error value e_xy */

	for(int i = 0; i < (int)view_distance; ++i)
	{  /* loop */
	//	setPixel(x0, y0);
		if(!map[start_x][start_y].getIsBlocked())
		{
			map[start_x][start_y].setIsVisible(true);
		}
		
		if(start_x == end_x && start_y == end_y) break;
		e2 = 2 * err;
		if(e2 >= dy) { err += dy; start_x += sx; } /* e_xy+e_x > 0 */
		if(e2 <= dx) { err += dx; start_y += sy; } /* e_xy+e_y < 0 */
	}
}
