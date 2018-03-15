#pragma once

//STD
#include <functional>
#include <utility>

//SELF
#include "Message.hpp"

class FunctionMessage : public Message
{
public:
	FunctionMessage(std::function<void()> func)
		: Message(ID)
		, function(std::move(func))
	{}

	void execute()
	{
		function();
	}

	static constexpr HashedID ID = hash("Function");

private:
	std::function<void()> function;
};