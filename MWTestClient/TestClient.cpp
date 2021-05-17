/*
* File:		TestClient.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#include "TestClient.h"

using namespace Milkweed;

void TestClient::init() {
	MWLOG(Info, TestClient, "Initialized scene");

	// Initialize the camera used to display sprites
	m_spriteCamera.init();

	// Initialize the shader used to display sprites
	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_spriteCamera);
}

void TestClient::enter() {
	MWLOG(Info, TestClient, "entered scene");
	std::string address = "127.0.0.1";
	unsigned int port = 2773;
	double timeout = 10.0;

	MW::NETWORK.connect(address, port);
	double startTime = glfwGetTime();
	while (glfwGetTime() - startTime < timeout && !m_connected) {
		m_connected = MW::NETWORK.isConnected();
	}

	if (m_connected) {
		MWLOG(Info, TestClient, "Connected");
	}
	else {
		MWLOG(Error, TestClient, "Failed to connect");
	}
}

void TestClient::draw() {
	if (!m_authorized) {
		return;
	}

	// Get pointers to all the sprites
	m_playerPointers.clear();
	for (const std::pair<unsigned int, Sprite>& player : m_players) {
		m_playerPointers.push_back(&(m_players[player.first]));
	}

	// Draw the players on the server
	MW::RENDERER.submit(m_playerPointers, &m_spriteShader);
}

#define PLAYER_SPEED 3.0f

void TestClient::processInput() {
	// Process network input here
	unsigned int messageCount = 0;
	while (messageCount < 10 && !MW::NETWORK.getMessagesIn().empty()) {
		NetMessage message = MW::NETWORK.getMessagesIn().popFront();
		processNetMessage(message);

		messageCount++;
	}

	// Check the connection and authorization before proceeding
	m_connected = MW::NETWORK.isConnected();
	if (!(m_connected && m_authorized)) {
		return;
	}

	// Move the player
	bool velocityChange = false;
	if (MW::INPUT.isKeyDown(K_D)) {
		if (m_players[m_playerID].velocity.x != PLAYER_SPEED) {
			velocityChange = true;
			m_players[m_playerID].velocity.x = PLAYER_SPEED;
		}
	}
	else if (MW::INPUT.isKeyDown(K_A)) {
		if (m_players[m_playerID].velocity.x != -PLAYER_SPEED) {
			velocityChange = true;
			m_players[m_playerID].velocity.x = -PLAYER_SPEED;
		}
	}
	else {
		if (m_players[m_playerID].velocity.x != 0.0f) {
			velocityChange = true;
			m_players[m_playerID].velocity.x = 0.0f;
		}
	}
	if (MW::INPUT.isKeyDown(K_W)) {
		if (m_players[m_playerID].velocity.y != PLAYER_SPEED) {
			velocityChange = true;
			m_players[m_playerID].velocity.y = PLAYER_SPEED;
		}
	}
	else if (MW::INPUT.isKeyDown(K_S)) {
		if (m_players[m_playerID].velocity.y != -PLAYER_SPEED) {
			velocityChange = true;
			m_players[m_playerID].velocity.y = -PLAYER_SPEED;
		}
	}
	else {
		if (m_players[m_playerID].velocity.y != 0.0f) {
			velocityChange = true;
			m_players[m_playerID].velocity.y = 0.0f;
		}
	}

	// Send a message if the velocity of the player changed
	if (velocityChange) {
		NetMessage message;
		message.header.ID = MOVEMENT;
		message << m_playerID << m_players[m_playerID].position
			<< m_players[m_playerID].velocity;
		MW::NETWORK.send(message);
	}
}

void TestClient::processNetMessage(NetMessage& message) {
	switch (message.header.ID) {
	case MessageTypes::ID_ASSIGNMENT: {
		// Read the message out
		unsigned int playerID;
		glm::vec3 spawnPosition;
		glm::vec2 spawnVelocity;
		message >> spawnVelocity >> spawnPosition >> playerID;

		// Set the playerID and generate this player in the renderer's list
		m_playerID = playerID;
		m_players[m_playerID] = Sprite();
		m_players[m_playerID].init(spawnPosition, glm::vec2(50.0f, 50.0f),
			MW::RESOURCES.getTexture("Assets/texture/self.png"));
		m_players[m_playerID].velocity = spawnVelocity;

		// The player now has been authorized with it's ID number
		std::cout << "Received ID: " << m_playerID << std::endl;
		m_authorized = true;
		break;
	}
	case MessageTypes::CONNECTION: {
		// Read the message out
		unsigned int playerID;
		glm::vec3 spawnPosition;
		glm::vec2 spawnVelocity;
		message >> spawnVelocity >> spawnPosition >> playerID;

		// Test if the player is already connected
		std::map<unsigned int, Sprite>::iterator it
			= m_players.find(playerID);
		if (it != m_players.end()) {
			return;
		}

		// The player must be added
		m_players[playerID] = Sprite();
		m_players[playerID].init(spawnPosition, glm::vec2(50.0f, 50.0f),
			MW::RESOURCES.getTexture("Assets/texture/other.png"));
		m_players[playerID].velocity = spawnVelocity;

		// Send a connection message back to the new player
		NetMessage message;
		message.header.ID = MessageTypes::CONNECTION;
		message << m_playerID << m_players[m_playerID].position
			<< m_players[m_playerID].velocity;
		MW::NETWORK.send(message);

		break;
	}
	case MessageTypes::MOVEMENT: {
		// Read out the message
		unsigned int playerID;
		glm::vec3 position;
		glm::vec2 velocity;
		message >> velocity >> position >> playerID;

		// Locate the player by its ID and update it
		std::map<unsigned int, Sprite>::iterator it
			= m_players.find(playerID);
		if (it == m_players.end()) {
			return;
		}
		m_players[playerID].position = position;
		m_players[playerID].velocity = velocity;

		break;
	}
	case MessageTypes::DISCONNECTION: {
		// Read the message out
		unsigned int playerID;
		message >> playerID;
		
		// Find and erase the player with the ID
		std::map<unsigned int, Sprite>::iterator it
			= m_players.find(playerID);
		if (it == m_players.end()) {
			return;
		}
		m_players.erase(it);
		break;
	}
	}
}

void TestClient::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {

}

void TestClient::updateWindowSize() {
	// Refresh the camera
	m_spriteCamera.destroy();
	m_spriteCamera.init();
}

void TestClient::update(float deltaTime) {
	m_spriteCamera.position = m_players[m_playerID].position
		- glm::vec3(m_players[m_playerID].dimensions.x / 2.0f,
			m_players[m_playerID].dimensions.y / 2.0f, 0.0f);
	m_spriteCamera.update(deltaTime);

	for (const std::pair<unsigned int, Sprite>& player : m_players) {
		m_players[player.first].update(deltaTime);
	}
}

void TestClient::exit() {
	MWLOG(Info, TestClient, "Exited scene");
}

void TestClient::destroy() {
	MWLOG(Info, TestClient, "Destroyed scene");
}

int main(int argc, char** argv) {
	TestClient testClient;
	MW::Init("Test Client", glm::ivec2(800, 600), false, 60.0f, { &testClient },
		&testClient);
	testClient.destroy();
	return 0;
}