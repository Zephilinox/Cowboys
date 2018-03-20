#include "Pathfinding.h"

Pathfinding::Pathfinding(void)
{
	initialisedStartGoal = false;
	foundGoal = false;
}

Pathfinding::~Pathfinding(void)
{

}

void Pathfinding::FindPath(int currentPosX, int currentPosY, int targetPosX, int targetPosY)
{
	if (!initialisedStartGoal)
	{
		for (int i = 0; i < openList.size(); i++)
		{
			delete openList[i];
		}
		openList.clear();
		for (int i = 0; i < visitedList.size(); i++)
		{
			delete visitedList[i];
		}
		visitedList.clear();
		for (int i = 0; i < pathToGoal.size(); i++)
		{
			delete pathToGoal[i];
		}
		pathToGoal.clear();

		SearchCell start;
		start.xCoord = currentPosX;
		start.yCoord = currentPosY;

		SearchCell goal;
		goal.xCoord = targetPosX;
		goal.yCoord = targetPosY;

		SetStartAndGoal(start, goal);
		initialisedStartGoal = true;

		if (initialisedStartGoal)
		{
			ContinuePath();
		}
	}
	
}
void Pathfinding::SetStartAndGoal(SearchCell start, SearchCell goal)
{
	startCell = new SearchCell(start.xCoord, start.yCoord, NULL);
	goalCell = new SearchCell(goal.xCoord, goal.yCoord, &goal);

	startCell->G = 0;
	startCell->H = startCell->ManHattanDistance(goalCell);
	startCell->parent = 0;

	openList.push_back(startCell);

}

SearchCell* PathFinding::GetNextCell()
{
	float bestF = 99999.0f;
	int cellIndex = -1;
	SearchCell* nextCell = NULL;
	for (int i = 0; i < openList.size(); i++)
	{
		if (openList[i]->GetF() < bestF)
		{
			bestF = openList[i]->GetF();
			cellIndex = i;
		}

		if (cellIndex >= 0)
		{
			nextCell = openList[cellIndex];
			visitedList.push_back(nextCell);
			openList.erase(openList.begin() + cellIndex);
			
		}
		return nextCell;
	}
}