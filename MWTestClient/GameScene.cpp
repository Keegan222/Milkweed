/*
* File:		GameScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#include "GameScene.h"

using namespace Milkweed;

void GameScene::init() {
	MWLOG(Info, GameScene, "Initialized scene");

	// Initialize the camera used to display sprites
	m_spriteCamera.init();

	// Initialize the shader used to display sprites
	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_spriteCamera);
}

void GameScene::enter() {
	MWLOG(Info, GameScene, "entered scene");
	// Attempt to connect to the server
	MW::NETWORK.connect(m_address, m_port);
}

void GameScene::draw() {
	if (!(m_connected && m_authorized)) {
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

void GameScene::processInput() {
	// Process network input here
	unsigned int messageCount = 0;
	while (messageCount < 10 && !MW::NETWORK.getMessagesIn().empty()) {
		NetMessage message = MW::NETWORK.getMessagesIn().popFront();
		processNetMessage(message);

		messageCount++;
	}

	// Check the connection and authorization before proceeding
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
		message.header.ID = PLAYER_MOVEMENT;
		message << m_playerID << m_players[m_playerID].position
			<< m_players[m_playerID].velocity;
		MW::NETWORK.send(message);
	}
}

void GameScene::processNetMessage(NetMessage& message) {
	std::cout << "Received message " << message << std::endl;
	switch (message.header.ID) {
	case NetMessageTypes::CONNECTED: {
		MWLOG(Info, GameScene, "CONNECTED message received");
		m_connected = true;
		break;
	}
	case NetMessageTypes::DISCONNECTED: {
		MWLOG(Info, GameScene, "DISCONNECTED message received");
		m_connected = false;
		m_authorized = false;
		m_playerID = 0;
		m_players.clear();
		for (unsigned int i = 0; i < m_playerPointers.size(); i++) {
			m_playerPointers[i] = nullptr;
		}
		m_playerPointers.clear();
		// Repeated attempt to reconnect
		// TODO: Remove this later
		MW::NETWORK.disconnect();
		break;
	}
	case MessageTypes::PLAYER_ID_ASSIGNMENT: {
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
	case MessageTypes::PLAYER_CONNECTED: {
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
		message.header.ID = MessageTypes::PLAYER_CONNECTED;
		message << m_playerID << m_players[m_playerID].position
			<< m_players[m_playerID].velocity;
		MW::NETWORK.send(message);

		break;
	}
	case MessageTypes::PLAYER_MOVEMENT: {
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
	case MessageTypes::PLAYER_DISCONNECTED: {
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

void GameScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {

}

void GameScene::updateWindowSize() {
	// Refresh the camera
	m_spriteCamera.destroy();
	m_spriteCamera.init();
}

void GameScene::update(float deltaTime) {
	m_spriteCamera.position = m_players[m_playerID].position
		- glm::vec3(m_players[m_playerID].dimensions.x / 2.0f,
			m_players[m_playerID].dimensions.y / 2.0f, 0.0f);
	m_spriteCamera.update(deltaTime);

	for (const std::pair<unsigned int, Sprite>& player : m_players) {
		m_players[player.first].update(deltaTime);
	}
}

void GameScene::exit() {
	MWLOG(Info, GameScene, "Exited scene");
}

void GameScene::destroy() {
	MWLOG(Info, GameScene, "Destroyed scene");
}