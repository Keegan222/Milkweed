/*
* File:		ConnectScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.22
*/

#include "TestClient.h"
#include "ConnectScene.h"

void ConnectScene::init() {
	// Set up camera
	m_UICamera.init();
	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);

	// Set up shaders
	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);
	m_textShader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);

	// Set up the main UI group
	m_mainUIGroup.init(this, MAIN_UI_GROUP, MW::RESOURCES.getFont(
		"Assets/font/arial.ttf"), &m_spriteShader, &m_textShader, "textColor");

	// Set up UI variables
	glm::vec2 winDims = glm::vec2(800, 600);
	glm::vec2 textBoxDims = glm::vec2(300.0f, 30.0f) / winDims;
	glm::vec2 buttonDims = glm::vec2(200.0f, 30.0f) / winDims;
	float cursorWidth = 1.0f;
	float margin = 3.0f / 800.0f;
	float buffer = 0.05f;
	Texture* textBoxTexture = MW::RESOURCES.getTexture(
		"Assets/texture/text_box.png");
	Texture* cursorTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cursor.png");
	Texture* buttonTexture = MW::RESOURCES.getTexture(
		"Assets/texture/button.png");
	float textScale = 0.25f * ((float)MW::WINDOW.getDimensions().y / winDims.y);
	glm::vec3 textColor = glm::vec3(0.75f, 0.75f, 0.75f);

	// Set up UI components
	m_addressBox.init("Address", "", glm::vec3(0.5f - textBoxDims.x / 2.0f, 0.5f,
		0.0f), textBoxDims, cursorWidth, margin, textScale, textColor,
		Justification::LEFT, Justification::CENTER, textBoxTexture,
		cursorTexture, 100);
	m_portBox.init("Port", "", glm::vec3(0.5f - textBoxDims.x / 2.0f,
		0.5f - (textBoxDims.y + buffer), 0.0f), textBoxDims, cursorWidth, margin,
		textScale, textColor, Justification::LEFT, Justification::CENTER,
		textBoxTexture, cursorTexture, 100);
	m_backButton.init("Back", glm::vec3(0.0f, 0.0f, 0.0f), buttonDims,
		textScale, textColor, Justification::CENTER, Justification::CENTER,
		buttonTexture);
	m_defaultsButton.init("Defaults", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.0f, 0.0f), buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_connectButton.init("Connect", glm::vec3(1.0f - buttonDims.x, 0.0f, 0.0f),
		buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponents({ &m_addressBox, &m_portBox, &m_backButton,
		&m_defaultsButton, &m_connectButton });
}

void ConnectScene::enter() {
	// Set up directions for gamepads
	if (MW::INPUT.getGamepadCount() > 0) {
		m_addressBox.setDirections(nullptr, &m_portBox, nullptr, nullptr);
		m_portBox.setDirections(&m_addressBox, &m_defaultsButton, nullptr,
			nullptr);
		m_backButton.setDirections(&m_portBox, nullptr, nullptr,
			&m_defaultsButton);
		m_defaultsButton.setDirections(&m_portBox, nullptr, &m_backButton,
			&m_connectButton);
		m_connectButton.setDirections(&m_portBox, nullptr, &m_defaultsButton,
			nullptr);
		m_mainUIGroup.setSelectedComponent(&m_addressBox);
	}
	m_mainUIGroup.setEnabled(true);
	
	m_addressBox.setText(Options::DEFAULT_ADDRESS);
	m_portBox.setText(std::to_string(Options::DEFAULT_PORT));
}

void ConnectScene::draw() {
	m_mainUIGroup.draw();
}

void ConnectScene::processInput() {
	m_mainUIGroup.processInput();
}

void ConnectScene::componentEvent(unsigned int groupID,
	unsigned int componentID, unsigned int eventID) {
	switch (groupID) {
	case MAIN_UI_GROUP: {
		// Clear old networking messages
		MW::NETWORK.getMessagesIn().clear();
		// Process button clicks on the main group
		if (componentID == m_backButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				MW::SetScene(&TestClient::TITLE_SCENE);
			}
		}
		else if (componentID == m_defaultsButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				m_addressBox.setText(Options::DEFAULT_ADDRESS);
				m_portBox.setText(std::to_string(Options::DEFAULT_PORT));
			}
		}
		else if (componentID == m_connectButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				// Attempt to connect to the server by switching to the game
				// scene
				TestClient::GAME_SCENE.setAddress(m_addressBox.getText());
				TestClient::GAME_SCENE.setPort(std::atoi(
					m_portBox.getText().c_str()));
				MWLOG(Info, ConnectScene, "Connecting to ",
					m_addressBox.getText(), " on port ", m_portBox.getText());
				MW::SetScene(&TestClient::GAME_SCENE);
			}
		}
		break;
	}
	}
}

void ConnectScene::updateWindowSize() {
	m_mainUIGroup.updateWindowSize();
}

void ConnectScene::update(float deltaTime) {
	m_mainUIGroup.update(deltaTime);
	m_UICamera.update(deltaTime);
}

void ConnectScene::exit() {
	m_mainUIGroup.setEnabled(false);
}

void ConnectScene::destroy() {
	m_UICamera.destroy();
	m_spriteShader.destroy();
	m_textShader.destroy();
	m_mainUIGroup.destroy();
}