#pragma once
#include "Action.h"
#include <queue>

class ActionQueue
{
public:
	ActionQueue() = default;
	~ActionQueue() = default;

	Action getNextAction();
	void removeCurrentAction();
	void addToQueue(Action new_action);
	unsigned int getSize();

private:
	std::queue<Action> queue;
};
