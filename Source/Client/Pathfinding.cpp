#include "Pathfinding.h"

Pathfinding::Pathfinding()
{
	initialisedStartGoal = false;
	foundGoal = false;
}

void Pathfinding::FindPath(int currentPosX, int currentPosY, int targetPosX, int targetPosY)
{
	if (!initialisedStartGoal)
	{
		for (size_t i = 0; i < openList.size(); i++)
		{
			delete openList[i];
		}
		openList.clear();

		for (size_t i = 0; i < visitedList.size(); i++)
		{
			delete visitedList[i];
		}
		visitedList.clear();

		initialisedStartGoal = true;

		if (initialisedStartGoal)
		{
			ContinuePath();
		}
	}
}

void Pathfinding::setStartAndGoal(SearchCell start, SearchCell goal)
{
	startCell = new SearchCell(start.xCoord, start.yCoord, NULL);
	goalCell = new SearchCell(goal.xCoord, goal.yCoord, &goal);

	startCell->dist_from_start = 0;
	startCell->dist_from_goal = startCell->ManHattanDistance(goalCell);
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
		if (openList[i]->getCombinedCost() < bestF)
		{
			bestF = openList[i]->getCombinedCost();
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
	
	int id = y * 5 + x;

	for (size_t i = 0; i < visitedList.size(); i++)
	{
		if (id == visitedList[i]->id)
		{
			return;
		}
	}
	SearchCell* newChild = new SearchCell(x, y, parent);
	newChild->dist_from_start = newCost;
	newChild->dist_from_goal = parent->ManHattanDistance(goalCell);

	for (size_t i = 0; i < openList.size(); i++)
	{
		if (id == openList[i]->id)
		{
			float newF = newChild->dist_from_start = newCost + openList[i]->dist_from_goal;

			if (openList[i]->getCombinedCost() > newF)
			{
				openList[i]->dist_from_start = newChild->dist_from_start + newCost;
				openList[i]->parent = newChild;
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

	if (currentCell->id == goalCell->id)
	{
		goalCell->parent = currentCell->parent;
		SearchCell* getPath;
	}

	currentCell = GetNextCell();
}





