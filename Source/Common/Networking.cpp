#include "stdafx.h"

#include "Networking.h"
#include "Tracer.h"

NetworkComponent::NetworkComponent() 
{
	enetpp::global_state::get().initialize();
}

NetworkComponent::~NetworkComponent() 
{
	enetpp::global_state::get().deinitialize();
}

void NetworkComponent::trace(const std::string && msg)
{
	PrintThread{} << msg << std::endl;
}

void NetworkComponent::trace(const std::string & msg)
{
	PrintThread{} << msg << std::endl;
}

