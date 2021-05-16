/*
* File:		TestServer.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#include "TestServer.h"

using namespace Milkweed;

TestServer::~TestServer() {
	destroy();
}

bool TestServer::onConnect(std::shared_ptr<NetConnection> client) {
	std::cout << "Connected client " << client->getID() << std::endl;
	return true;
}

void TestServer::onMessage(NetMessage& message) {
	std::cout << "Message from client " << message.owner->getID() << std::endl
		<< message << std::endl;
}

void TestServer::onDisconnect(std::shared_ptr<NetConnection> client) {
	std::cout << "Disconnected client " << client->getID() << std::endl;
}

int main(int argc, char** argv) {
	unsigned int port = 2773;

	TestServer testServer(port);
	testServer.init();
	while (true) {
		testServer.update(-1);
	}
}