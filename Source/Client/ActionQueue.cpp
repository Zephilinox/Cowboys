#include "ActionQueue.h"
#include <vector>

Action ActionQueue::getNextAction()
{
	return queue.front();
}

void ActionQueue::removeCurrentAction()
{
	queue.pop();
}

void ActionQueue::addToQueue(Action new_action)
{
	queue.push(std::move(new_action));
}

unsigned int ActionQueue::getSize()
{
	return queue.size();
}
