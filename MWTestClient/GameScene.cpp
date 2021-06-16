/*
* File:		GameScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#include <MWTestServer/NetTypes.h>

#include "TestClient.h"
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

	// Initialize the UI camera
	m_UICamera.init();
	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);

	// Initialize the text shader
	m_UISpriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);
	m_UITextShader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);

	// Initialize UI component variables
	Font* font = MW::RESOURCES.getFont("Assets/font/arial.ttf");
	float buffer = 0.015f;
	glm::vec2 cWinDims = MW::WINDOW.getDimensions();
	glm::vec2 winDims = glm::vec2(800, 600);
	glm::vec2 buttonDims = glm::vec2(200.0f / winDims.x - 2 * buffer,
		30.0f / winDims.y);
	glm::vec2 backgroundDims = glm::vec2(200.0f / winDims.x,
		buttonDims.y + 2 * buffer);
	float cursorWidth = 1.0f;
	Texture* backgroundTexture = MW::RESOURCES.getTexture(
		"Assets/texture/pause_background.png");
	Texture* buttonTexture
		= MW::RESOURCES.getTexture("Assets/texture/button.png");
	Texture* textAreaTexture = MW::RESOURCES.getTexture(
		"Assets/texture/text_area.png");
	Texture* cursorTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cursor.png");
	float textScale = 0.25f * ((float)MW::WINDOW.getDimensions().y / winDims.y);
	glm::vec3 textColor = glm::vec3(0.75f, 0.75f, 0.75f);

	// Initialize pause UI components
	m_pauseUIGroup.init(this, PAUSE_UI_GROUP, font, &m_UISpriteShader,
		&m_UITextShader, "textColor");
	m_pauseBackground.init(glm::vec3((cWinDims.x - backgroundDims.x) / 2.0f,
		cWinDims.y / 2.0f, 1.0f), backgroundDims, backgroundTexture);
	m_optionsButton.init("Options", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f + (buffer + buttonDims.y + buffer), 2.0f), buttonDims, textScale,
		textColor, Justification::CENTER, Justification::CENTER,
		buttonTexture);
	m_disconnectButton.init("Disconnect", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f + buffer, 2.0f), buttonDims, textScale,
		textColor, Justification::CENTER, Justification::CENTER, buttonTexture);
	m_pauseUIGroup.addComponents({ &m_optionsButton, &m_disconnectButton });

	// Initialize HUD UI components
	m_HUDUIGroup.init(this, HUD_UI_GROUP, font, &m_UISpriteShader,
		&m_UITextShader, "textColor");
	m_statsArea.init("", 5, glm::vec3(0.75f, 0.75f, 0.0f),
		glm::vec2(0.25f, 0.25f), cursorWidth, textScale, textColor, Justification::LEFT,
		Justification::CENTER, nullptr, cursorTexture);
	m_HUDUIGroup.addComponent(&m_statsArea);
	updateStatsArea();
	m_statsArea.setEnabled(false);
}

void GameScene::enter() {
	MW::INPUT.addInputListener(this);
	MWLOG(Info, GameScene, "Entered scene");
	// Attempt to connect to the server
	if (!MW::NETWORK.isConnected()) {
		MW::NETWORK.connect(m_address, m_port);
	}

	// Set directions for the pause UI for controllers
	if (MW::INPUT.getGamepadCount() > 0) {
		setComponentDirections();
		m_pauseUIGroup.setSelectedComponent(&m_optionsButton);
		MW::WINDOW.setCursorEnabled(false);
	}
	else {
		MW::WINDOW.setCursorEnabled(true);
		m_pauseUIGroup.setSelectedComponent(nullptr);
	}
	m_pauseUIGroup.setEnabled(false);
	m_pauseUIGroup.setVisible(false);

	m_statsArea.setEnabled(false);
}

#define SELF_DEPTH 0.1f

void GameScene::draw() {
	if (m_pauseMenuUp) {
		MW::RENDERER.submit({ &m_pauseBackground }, &m_UISpriteShader);
		m_pauseUIGroup.draw();
	}

	m_playerPointers.resize(m_players.size());
	int i = 0;
	for (const std::pair<unsigned int, ClientPlayer>& p : m_players) {
		m_playerPointers[i++] = &(m_players[p.first]);
	}
	m_players[m_playerID].position.z = SELF_DEPTH;
	MW::RENDERER.submit(m_playerPointers, &m_spriteShader);

	m_HUDUIGroup.draw();
}

#define PLAYER_SPEED 3.0f

void GameScene::processInput() {
	if (MW::INPUT.isKeyPressed(K_G)) {
		MW::RENDERER.dumpNextFrame();
	}

	// Check for the escape menu
	if (MW::INPUT.isKeyPressed(F_ESCAPE)
		|| MW::INPUT.isGamepadButtonPressed(G_BACK)) {
		m_pauseMenuUp = !m_pauseMenuUp;
		if (m_pauseMenuUp) {
			m_pauseUIGroup.setEnabled(true);
			m_pauseUIGroup.setVisible(true);
		}
		else {
			m_pauseUIGroup.setEnabled(false);
			m_pauseUIGroup.setVisible(false);
		}
	}

	m_pauseUIGroup.processInput();
	m_HUDUIGroup.processInput();

	if (MW::INPUT.isKeyPressed(F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}

	if (!(m_connected && m_accepted) || m_pauseMenuUp) {
		return;
	}

	// The client is connected
	if (MW::INPUT.isKeyPressed(K_P)) {
		NetMessage p;
		p.header.ID = MessageTypes::PING;
		p << m_playerID;
		MW::NETWORK.send(p);
	}

	m_players[m_playerID].processInput();
}

void GameScene::gamepadConnected(int gp) {
	MW::WINDOW.setCursorEnabled(false);
	if (m_pauseUIGroup.getSelectedComponent() == nullptr) {
		setComponentDirections();
		m_pauseUIGroup.setSelectedComponent(&m_optionsButton);
	}
}

void GameScene::gamepadDisconnected(int gp) {
	if (MW::INPUT.getGamepadCount() == 1) {
		MW::WINDOW.setCursorEnabled(true);
		m_pauseUIGroup.setSelectedComponent(nullptr);
	}
}

void GameScene::processNetMessage(NetMessage& message) {
	switch (message.header.ID) {
	case NetMessageTypes::CONNECTED: {
		MWLOG(Info, GameScene, "Received CONNECTED system network message");
		m_connected = true;
		break;
	}
	case NetMessageTypes::DISCONNECTED: {
		MWLOG(Info, GameScene, "Recieved DISCONNECTED system network message");
		disconnect();
		break;
	}
	case NetMessageTypes::FAILED: {
		MWLOG(Info, GameScene, "Received FAILED system network message");
		disconnect();
		break;
	}
	case MessageTypes::ACCEPT_PLAYER: {
		// Initialize this player
		m_players.clear();
		m_playerID = 0;
		message >> m_playerID;
		m_players.emplace(m_playerID, ClientPlayer());
		m_players[m_playerID].init(this, m_playerID);

		// Initialize previously connected players
		int otherCount = 0;
		message >> otherCount;
		for (int i = 0; i < otherCount; i++) {
			unsigned int playerID = 0;
			glm::vec3 pos = glm::vec3();
			glm::vec2 vel = glm::vec2();
			message >> vel >> pos >> playerID;
			m_players.emplace(playerID, ClientPlayer());
			m_players[playerID].init(this, playerID);
			m_players[playerID].position = pos;
			m_players[playerID].velocity = vel;
		}

		m_accepted = true;

		break;
	}
	case MessageTypes::CONNECT_PLAYER: {
		// Initialize a blank player with that ID
		unsigned int playerID = 0;
		message >> playerID;
		m_players.emplace(playerID, ClientPlayer());
		m_players[playerID].init(this, playerID);
		break;
	}
	case MessageTypes::PING: {
		unsigned int playerID = 0;
		message >> playerID;
		break;
	}
	case MessageTypes::PLAYER_PV_UPDATE: {
		glm::vec2 velocity = glm::vec2();
		glm::vec3 position = glm::vec3();
		unsigned int playerID = 0;
		message >> velocity >> position >> playerID;
		m_players[playerID].position = position;
		m_players[playerID].velocity = velocity;
		break;
	}
	case MessageTypes::DISCONNECT_PLAYER: {
		unsigned int playerID = 0;
		message >> playerID;
		// Search for the player with that ID and clear it out if found
		std::map<unsigned int, ClientPlayer>::iterator it
			= m_players.find(playerID);
		if (it != m_players.end()) {
			m_players.erase(it);
		}
		break;
	}
	}
}

void GameScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	if (groupID == PAUSE_UI_GROUP) {
		if (componentID == m_optionsButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				m_pauseMenuUp = false;
				m_pauseUIGroup.setEnabled(false);
				m_pauseUIGroup.setVisible(false);
				TestClient::OPTIONS_SCENE.setReturnScene(this);
				MW::SetScene(&TestClient::OPTIONS_SCENE);
			}
		}
		else if (componentID == m_disconnectButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				m_pauseMenuUp = false;
				m_pauseUIGroup.setEnabled(false);
				m_pauseUIGroup.setVisible(false);
				disconnect();
			}
		}
	}
}

void GameScene::updateWindowSize() {
	m_HUDUIGroup.updateWindowSize();
	glm::vec2 resizeScale = m_pauseUIGroup.updateWindowSize();
	m_pauseBackground.dimensions *= resizeScale;
	m_pauseBackground.position.x *= resizeScale.x;
	m_pauseBackground.position.y *= resizeScale.y;

	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);

	m_spriteCamera.destroy();
	m_spriteCamera.init();
}

void GameScene::update(float deltaTime) {
	updateStatsArea();
	m_HUDUIGroup.update(deltaTime);
	m_pauseUIGroup.update(deltaTime);

	for (const std::pair<unsigned int, ClientPlayer>& player : m_players) {
		m_players[player.first].update(deltaTime);
	}
	m_spriteCamera.position = m_players[m_playerID].position;

	m_spriteCamera.update(deltaTime);
	m_UICamera.update(deltaTime);
}

void GameScene::exit() {
	MW::INPUT.removeInputListener(this);
	MWLOG(Info, GameScene, "Exited scene");
	m_pauseMenuUp = false;
	m_pauseUIGroup.setEnabled(false);
	m_pauseUIGroup.setVisible(false);
	m_HUDUIGroup.setEnabled(false);
}

void GameScene::destroy() {
	MWLOG(Info, GameScene, "Destroyed scene");
	m_spriteShader.destroy();
	m_UITextShader.destroy();
	m_UISpriteShader.destroy();
	m_UICamera.destroy();
	m_spriteCamera.destroy();
	m_pauseUIGroup.destroy();
	m_pauseBackground.destroy();
	m_HUDUIGroup.destroy();
}

void GameScene::updateStatsArea() {
	std::ostringstream stream;
	stream << "Player ID: " << m_playerID << std::endl;
	stream << "Player count: " << m_players.size() << std::endl;
	stream << "Position: (" << m_players[m_playerID].position.x << ", "
		<< m_players[m_playerID].position.y << ")" << std::endl;

	m_statsArea.setText(stream.str());
}

void GameScene::disconnect() {
	m_connected = false;
	m_accepted = false;
	m_playerID = 0;
	m_players.clear();
	MW::NETWORK.disconnect();
	MW::SetScene(&TestClient::CONNECT_SCENE);
}

void GameScene::setComponentDirections() {
	m_optionsButton.setDirections(nullptr, &m_disconnectButton, nullptr,
		nullptr);
	m_disconnectButton.setDirections(&m_optionsButton, nullptr, nullptr,
		nullptr);
}