#pragma once

//STD
#include <functional>
#include <utility>

//SELF
#include "Connection.hpp"

/*
The Slot struct is just a replacement for std::pair.
I prefer to refer to named variables rather than first and second.
*/
template <typename... Args>
struct Slot
{
	Slot(Connection c, std::function<void(Args...)> f)
		: connection(c)
		, function(std::move(f))
	{}

	Connection connection;
	std::function<void(Args...)> function;
};