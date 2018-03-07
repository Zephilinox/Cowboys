#pragma once
#include <enetpp/client.h>
#include <Common/Networking.h>

class ClientComponent :
	public NetworkComponent
{
public:
	ClientComponent() = default;
	~ClientComponent() = default;

	// Inherited via NetworkComponent
	virtual bool initialize() override;
	virtual bool deinitialize() override;
	virtual void consumeEvents() override;

	bool isConnected() const;
	bool isConnecting() const;

private:
	// the client thread
	enetpp::client client;

	// three consume functions to process networking
	std::function<void()> on_connected;
	std::function<void()> on_disconnected;
	std::function<void(const enet_uint8* data, size_t data_size)> on_data_received;
	
	int channel_count = 1;
};
