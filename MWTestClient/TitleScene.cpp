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
	glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);

	m_connectButton.init("Connect", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f, 0.0f), buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponent(&m_connectButton);

	m_optionsButton.init("Options", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f - (buttonDims.y + buffer), 0.0f), buttonDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponent(&m_optionsButton);

	m_quitButton.init("Quit", glm::vec3(0.5f - buttonDims.x / 2.0f,
		0.5f - 2 * (buttonDims.y + buffer), 0.0f), buttonDims, textScale,
		textColor, Justification::CENTER, Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponent(&m_quitButton);

	m_initialized = true;
}

void TitleScene::enter() {
	m_mainUIGroup.enable();
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

void TitleScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	MWLOG(Info, TitleScene, "Event ", eventID, " on component ", componentID,
		" from group ", groupID);

	switch (groupID) {
	case MAIN_UI_GROUP: {
		switch (eventID) {
		case UI::Button::CLICKED_EVENT: {
			if (componentID == m_connectButton.getID()) {
				MWLOG(Info, TitleScene, "Swapping to connect scene");
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
	MWLOG(Info, TitleScene, "Updated window size");

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
	m_mainUIGroup.disable();
}

void TitleScene::destroy() {
	m_UICamera.destroy();
	m_spriteShader.destroy();
	m_textShader.destroy();
	m_mainUIGroup.destroy();
	m_initialized = false;
}