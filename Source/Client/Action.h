#pragma once

enum ActionID
{
	END_TURN,
	ATTACK,
	REACTIVE_ATTACK,
	MOVE
};

struct Action
{
	ActionID action_id;
	uint32_t actor_id;
	uint32_t defender_id;
	bool isHit;
	bool isReactive;
	bool reactiveHit;
	int damage;
	std::pair<int, int> start_pos_x_y;
	std::pair<int, int> end_pos_x_y;
};