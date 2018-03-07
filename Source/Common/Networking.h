#pragma once
#include <string>
#include <enetpp\global_state.h>
#include "Component.h"

class NetworkComponent :
	public Component
{
 public:
	 NetworkComponent();
	 virtual ~NetworkComponent();

	 virtual bool initialize() = 0;
	 virtual bool deinitialize() = 0;
	 virtual void consumeEvents() = 0;

	 void trace(const std::string&& msg);
	 void trace(const std::string& msg);
};
