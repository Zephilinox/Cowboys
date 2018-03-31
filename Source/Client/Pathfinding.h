#pragma once
#include <vector>
#include <iostream>
#include "Grid.h"
#include "SearchCell.h"
#include <iostream>

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();

	void FindPath(int currentPosX, int currentPosY, int targetPosX, int targetPosY);
//	float NextPathPosX();
//	float NextPathPosY();

	void ClearOpenList() { openList.clear(); }
	void ClearVisistedList() { visitedList.clear(); }
	//void ClearPathToGoal() { pathToGoal.clear(); }
	bool initialisedStartGoal;
	bool foundGoal;

private:
	void setStartAndGoal(SearchCell start, SearchCell goal);
	void PathOpened(int x, int y, float newCost, SearchCell *parent);
	SearchCell * GetNextCell();
	void ContinuePath();

	SearchCell *startCell;
	SearchCell *goalCell;
	SearchCell* currentCell;

	std::vector<SearchCell*> openList;
	std::vector<SearchCell*> visitedList;
	//std::vector<std::pair<int,int>> pathToGoal;
};