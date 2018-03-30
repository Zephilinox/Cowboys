#include "Pathfinding.h"

Pathfinding::Pathfinding()
{
	initialisedStartGoal = false;
	foundGoal = false;
}

Pathfinding::~Pathfinding()
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

SearchCell* Pathfinding::GetNextCell()
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

void Pathfinding::PathOpened(int x, int y, float newCost, SearchCell *parent)
{
	
	int id = y * GRID_SIZE + x;

	for (int i = 0; i < visitedList.size(); i++)
	{
		if (id == visitedList[i]->id)
		{
			return;
		}
	}
	SearchCell* newChild = new SearchCell(x, y, parent);
	newChild->G = newCost;
	newChild->H = parent->ManHattanDistance(goalCell);

	for (int i = 0; i < openList.size(); i++)
	{
		if (id == openList[i]->id)
		{
			float newF = newChild->G = newCost + openList[i]->H;

			if (openList[i]->GetF() > newF)
			{
				openList[i]->G = newChild->G + newCost;
				openList[1]->parent = newChild;
			}
			else
			{
				delete newChild;
				return;
			}
		}
	}

	openList.push_back(newChild);

}
void Pathfinding::ContinuePath()
{
	if (openList.empty())
	{
		return;
	}
	SearchCell* currentCell = GetNextCell();

	if (currentCell->id == goalCell->id)
	{
		goalCell->parent = currentCell->parent;
		SearchCell* getPath;

		for (getPath = goalCell; getPath != NULL; getPath = getPath->parent)
		{

		}
	}
}