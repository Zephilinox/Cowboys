#pragma once
class Component
{
	enum class ComponentType
	{
		INVALID = 0,
		NETWORKING = 1
	};

public:
	Component() = default;
	~Component() = default;

	ComponentType getType() { return id; }

private:
	ComponentType id;
};