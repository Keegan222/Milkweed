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
	m_players[newClientID] = ServerPlayer();
	m_players[newClientID].init(this, newClientID, PLAYER_SPAWNPOINT,
		glm::vec2(0.0f, 0.0f));
	m_players[newClientID].m_client = client;

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
		if (clientID != newClientID) {
			cmsg << m_players[clientID].velocity
				<< m_players[clientID].position << clientID;
		}
	}
	int otherCount = m_players.size() - 1;
	cmsg << otherCount;
	cmsg << newClientID;
	messageClient(client, cmsg);

	// Send all other players' usernames to the new player
	for (const std::pair<unsigned int, ServerPlayer>& player : m_players) {
		if (player.first != newClientID) {
			publishPlayerUsername(player.first, newClientID);
		}
	}

	return true;
}

void TestServer::onMessage(NetMessage& message) {
	switch (message.header.ID) {
	case MessageTypes::USERNAME_REQUEST: {
		unsigned int clientID = message.owner->getID();
		int length = 0;
		message >> length;
		if (length < 0) {
			length = 0;
			break;
		}
		else if (length > 20) {
			length = 20;
		}
		std::string username;
		username.resize(length);
		char c;
		for (int i = 0; i < length; i++) {
			message >> c;
			if (c == 0) {
				username.resize(i);
				break;
			}
			username[i] = c;
		}
		SERVERLOG(Info, "Received username \"", username, "\" (", length,
			" characters) from client ", clientID);
		m_players[clientID].username = username;
		publishPlayerUsername(clientID);
		break;
	}
	case MessageTypes::PING: {
		SERVERLOG(Info, "Pinging all clients on request of client ",
			message.owner->getID());
		messageAllClients(message);
		break;
	}
	case MessageTypes::MOVEMENT_LEFT: {
		sendPlayerPVUpdate(message);
		break;
	}
	case MessageTypes::MOVEMENT_STOP_LEFT: {
		sendPlayerPVUpdate(message);
		break;
	}
	case MessageTypes::MOVEMENT_RIGHT: {
		sendPlayerPVUpdate(message);
		break;
	}
	case MessageTypes::MOVEMENT_STOP_RIGHT: {
		sendPlayerPVUpdate(message);
		break;
	}
	case MessageTypes::MOVEMENT_JUMP: {
		unsigned int clientID = message.owner->getID();
		if (!m_players[clientID].jumping) {
			m_players[clientID].velocity.y = PLAYER_JUMP_SPEED;
			m_players[clientID].jumping = true;
			sendPlayerPVUpdate(message);
		}
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

void TestServer::publishPlayerUsername(unsigned int clientID, int destID) {
	// Construct the username assignment message
	NetMessage umsg;
	umsg.header.ID = MessageTypes::USERNAME_ASSIGNMENT;
	const char* username = m_players[clientID].username.c_str();
	int length = m_players[clientID].username.length();
	for (int i = length - 1; i >= 0; i--) {
		umsg << username[i];
	}
	umsg << length;
	umsg << clientID;

	if (destID == -1) {
		SERVERLOG(Info, "Publishing username of ", clientID, " to all clients");
		messageAllClients(umsg);
	}
	else {
		SERVERLOG(Info, "Publishing username of ", clientID, " to client ",
			destID);
		messageClient(m_players[destID].m_client, umsg);
	}
}

void TestServer::sendPlayerPVUpdate(NetMessage& message) {
	unsigned int clientID = message.owner->getID();
	glm::vec3 position = glm::vec3();
	glm::vec2 velocity = glm::vec2();
	message >> position >> velocity;

	if (validatePlayerMovement(clientID, position, velocity)) {
		m_players[clientID].position = position;
		m_players[clientID].velocity = velocity;
	}

	NetMessage pvUpdate;
	pvUpdate.header.ID = MessageTypes::PLAYER_PV_UPDATE;
	pvUpdate << clientID << m_players[clientID].position
		<< m_players[clientID].velocity;
	messageAllClients(pvUpdate, m_players[clientID].m_client);
}

bool TestServer::validatePlayerMovement(unsigned int clientID,
	const glm::vec3& position, const glm::vec2& velocity) {
	// Ensure that the requested movement is within the bounds of the player's
	// abilities.
	return true;
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