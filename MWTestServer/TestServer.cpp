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
	// Send this client their ID number
	NetMessage assignMessage;
	assignMessage.header.ID = MessageTypes::PLAYER_ID_ASSIGNMENT;
	unsigned int clientID = client->getID();
	glm::vec3 spawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 spawnVelocity = glm::vec2(0.0f, 0.0f);
	assignMessage << clientID << spawnPosition << spawnVelocity;
	messageClient(client, assignMessage);

	// Send a new client message to all other clients
	NetMessage connectMessage;
	connectMessage.header.ID = MessageTypes::PLAYER_CONNECTED;
	connectMessage << clientID << spawnPosition << spawnVelocity;
	messageAllClients(connectMessage, client);

	return true;
}

void TestServer::onMessage(NetMessage& message) {
	std::cout << "Message from client " << message.owner->getID() << std::endl
		<< message << std::endl;

	switch (message.header.ID) {
	case MessageTypes::PLAYER_CONNECTED: {
		messageAllClients(message, message.owner);
		break;
	}
	case MessageTypes::PLAYER_MOVEMENT: {
		messageAllClients(message, message.owner);
		break;
	}
	}
}

void TestServer::onDisconnect(std::shared_ptr<NetConnection> client) {
	std::cout << "Disconnected client " << client->getID() << std::endl;
	NetMessage disconnectMessage;
	disconnectMessage.header.ID = MessageTypes::PLAYER_DISCONNECTED;
	unsigned int clientID = client->getID();
	disconnectMessage << clientID;
	messageAllClients(disconnectMessage, client);
}

int main(int argc, char** argv) {
	TestServer testServer(2773);
	testServer.init();
	while (true) {
		testServer.update(-1);
	}
}