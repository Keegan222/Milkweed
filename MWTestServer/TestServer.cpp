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

void TestServer::updatePhysics(float deltaTime) {
	for (std::map<unsigned int, ServerPlayer>::iterator it = m_players.begin();
		it != m_players.end(); ++it) {
		it->second.update(deltaTime);
	}
}

bool TestServer::onConnect(std::shared_ptr<NetConnection> client) {
	SERVERLOG(Info, "Connected player with ID ", client->getID());
	unsigned int newClientID = client->getID();
	m_players[newClientID] = ServerPlayer(newClientID,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));

	// Notify other players
	NetMessage omsg;
	omsg.header.ID = MessageTypes::CONNECT_PLAYER;
	omsg << newClientID;
	messageAllClients(omsg, client);
	SERVERLOG(Info, "Notified other players of connection");

	// Notify the player of connection acceptance
	NetMessage cmsg;
	cmsg.header.ID = MessageTypes::ACCEPT_PLAYER;
	for (const std::pair<unsigned int, ServerPlayer>& player : m_players) {
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

	// Remove the player
	std::map<unsigned int, ServerPlayer>::iterator it
		= m_players.find(oldClientID);
	if (it != m_players.end()) {
		m_players[oldClientID].destroy();
		m_players.erase(it);
	}
}

#define PHYSICS_UPS 60.0f

int main(int argc, char** argv) {
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	TestServer testServer(2773);
	if (!testServer.init()) {
		std::cout << "Failed to initialize server" << std::endl;
	}

	double startTime = glfwGetTime();
	unsigned int physicsSteps = 0, maxPhysicsSteps = 10;
	float physicsSPU = 1.0f / 60.0f;
	while (testServer.isActive()) {
		// Update the messages from the network
		testServer.update(-1);


		double now = glfwGetTime();
		float elapsed = (float)(now - startTime);
		startTime = now;
		float deltaTime = elapsed / physicsSPU;
		while (deltaTime > 1.0f && physicsSteps < maxPhysicsSteps) {
			testServer.updatePhysics(1.0f);
			deltaTime -= 1.0f;
			physicsSteps++;
		}
		testServer.updatePhysics(deltaTime);
		physicsSteps = 0;
	}

	return 0;
}