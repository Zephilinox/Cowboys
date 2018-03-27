#pragma once
#include <vector>
#include <iostream>
#include "Grid.h"
#include "SearchCell.h"
#include <iostream>
class Pathfinding
{
public:
	Pathfinding(void);
	~Pathfinding(void);

	void FindPath(int currentPosX, int currentPosY, int targetPosX, int targetPosY);
	float NextPathPosX();
	float NextPathPosY();

	void ClearOpenList() { openList.clear(); }
	void ClearVisistedList() { visitedList.clear; }
	void ClearPathToGoal() { pathToGoal.clear; }
	bool initialisedStartGoal;
	bool foundGoal;

private:
	void SetStartAndGoal(SearchCell start, SearchCell goal);
	void PathOpened(int x, int y, float newCost, SearchCell *parent);
	SearchCell * GetNextCell();
	void ContinuePath();

	SearchCell *startCell;
	SearchCell *goalCell;

	std::vector<SearchCell*> openList;
	std::vector<SearchCell*> visitedList;
	std::vector<int*> pathToGoal;
};