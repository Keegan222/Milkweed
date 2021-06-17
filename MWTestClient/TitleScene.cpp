/*
* File:		TitleScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#include "TitleScene.h"
#include "TestClient.h"

void TitleScene::init() {
	// Set up the camera
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

	// Set up the UI group
	m_mainUIGroup.init(this, MAIN_UI_GROUP,
		MW::RESOURCES.getFont("Assets/font/arial.ttf"), &m_spriteShader,
		&m_textShader, "textColor");

	// Set up and add the UI components
	glm::vec2 winDims = glm::vec2(800, 600);
	glm::vec2 buttonDims = glm::vec2(200.0f / winDims.x, 30.0f / winDims.y);
	float buffer = 0.015f;
	Texture* buttonTexture
		= MW::RESOURCES.getTexture("Assets/texture/button.png");
	float textScale = 0.25f * ((float)MW::WINDOW.getDimensions().y / winDims.y);
	glm::vec3 textColor = glm::vec3(0.75f, 0.75f, 0.75f);

	m_connectButton.init("Connect", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f, 0.0f), buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_optionsButton.init("Options", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f - (buttonDims.y + buffer), 0.0f), buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_quitButton.init("Quit", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f - 2 * (buttonDims.y + buffer), 0.0f), buttonDims, textScale,
		textColor, Justification::CENTER, Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponents({ &m_connectButton, &m_optionsButton,
			&m_quitButton });

	m_initialized = true;
}

void TitleScene::enter() {
	Options::INITIALIZED = true;
	MW::INPUT.addInputListener(this);
	// Set directions for controller input and add components
	if (MW::INPUT.getGamepadCount() > 0) {
		setComponentDirections();
		MW::WINDOW.setCursorEnabled(false);
	}
	else {
		m_mainUIGroup.setSelectedComponent(nullptr);
		MW::WINDOW.setCursorEnabled(true);
	}

	m_mainUIGroup.setEnabled(true);
}

void TitleScene::draw() {
	m_mainUIGroup.draw();
}

void TitleScene::processInput() {
	m_mainUIGroup.processInput();

	if (MW::INPUT.isKeyPressed(Key::F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}
}

void TitleScene::gamepadConnected(int gp) {
	MW::WINDOW.setCursorEnabled(false);
	if (m_mainUIGroup.getSelectedComponent() == nullptr) {
		setComponentDirections();
		m_mainUIGroup.setSelectedComponent(&m_connectButton);
	}
}

void TitleScene::gamepadDisconnected(int gp) {
	if (MW::INPUT.getGamepadCount() == 1) {
		MW::WINDOW.setCursorEnabled(true);
		m_mainUIGroup.setSelectedComponent(nullptr);
	}
}

void TitleScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	switch (groupID) {
	case MAIN_UI_GROUP: {
		switch (eventID) {
		case UI::Button::CLICKED_EVENT: {
			if (componentID == m_connectButton.getID()) {
				MW::SetScene(&TestClient::CONNECT_SCENE);
			}
			else if (componentID == m_optionsButton.getID()) {
				TestClient::OPTIONS_SCENE.setReturnScene(this);
				MW::SetScene(&TestClient::OPTIONS_SCENE);
			}
			else if (componentID == m_quitButton.getID()) {
				MW::RUNNING = false;
			}
			break;
		}
		}
		break;
	}
	}
}

void TitleScene::updateWindowSize() {
	// Update the UI positioning
	m_mainUIGroup.updateWindowSize();

	// Reset the camera
	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);
}

void TitleScene::update(float deltaTime) {
	m_mainUIGroup.update(deltaTime);
	m_UICamera.update(deltaTime);
}

void TitleScene::exit() {
	m_mainUIGroup.setEnabled(false);
	MW::INPUT.removeInputListener(this);
}

void TitleScene::destroy() {
	m_UICamera.destroy();
	m_spriteShader.destroy();
	m_textShader.destroy();
	m_mainUIGroup.destroy();
	m_initialized = false;
}

void TitleScene::setComponentDirections() {
	m_connectButton.setDirections(&m_quitButton, &m_optionsButton, nullptr,
		&m_optionsButton);
	m_optionsButton.setDirections(&m_connectButton, &m_quitButton,
		&m_connectButton, &m_quitButton);
	m_quitButton.setDirections(&m_optionsButton, &m_connectButton,
		&m_optionsButton, nullptr);
	m_mainUIGroup.setSelectedComponent(&m_connectButton);
}