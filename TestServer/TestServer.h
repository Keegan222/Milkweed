#pragma once

#include <Milkweed/MW.h>

class TestServer : public MW::NetServer {
public:
	TestServer(unsigned short port) : MW::NetServer(port) {}
	bool onConnect(std::shared_ptr<MW::NetConnection> client) override;
	void onMessage(const MW::NetMessage& message) override;
	void onDisconnect(std::shared_ptr<MW::NetConnection> client) override;
};