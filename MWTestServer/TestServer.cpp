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
	SERVERLOG(Info, "Connected player with ID ", client->getID());
	unsigned int newClientID = client->getID();
	m_players[newClientID] = Player();

	// Notify other players
	NetMessage omsg;
	omsg.header.ID = MessageTypes::CONNECT_PLAYER;
	omsg << newClientID;
	messageAllClients(omsg, client);
	SERVERLOG(Info, "Notified other players of connection");

	// Notify the player of connection acceptance
	NetMessage cmsg;
	cmsg.header.ID = MessageTypes::ACCEPT_PLAYER;
	for (const std::pair<unsigned int, Player>& player : m_players) {
		unsigned int clientID = player.first;
		cmsg << clientID;
	}
	cmsg << newClientID;
	messageClient(client, cmsg);
	SERVERLOG(Info, "Notified new player of acceptance");

	return true;
}

void TestServer::onMessage(NetMessage& message) {
	SERVERLOG(Info, message);
	switch (message.header.ID) {
	case MessageTypes::PING: {
		messageAllClients(message);
		break;
	}
	}
}

void TestServer::onDisconnect(std::shared_ptr<NetConnection> client) {
	SERVERLOG(Info, "Disconnected client ", client->getID());
	unsigned int oldClientID = client->getID();

	// Notify all connected clients of the disconnect
	NetMessage dmsg;
	dmsg.header.ID = MessageTypes::DISCONNECT_PLAYER;
	dmsg << oldClientID;
	messageAllClients(dmsg, client);
	SERVERLOG(Info, "Notified all clients of disconnect");
}

int main(int argc, char** argv) {
	TestServer testServer(2773);
	if (!testServer.init()) {
		std::cout << "Failed to initialize server" << std::endl;
	}

	while (testServer.isActive()) {
		testServer.update(-1);
	}

	return 0;
}