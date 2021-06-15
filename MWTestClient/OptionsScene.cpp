/*
* File:		OptionsScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#include "OptionsScene.h"

// Define the keys for the options
#define OPTIONS_FILE_NAME "test_client_options.ini"
#define DEFAULT_ADDRESS_KEY "default_address"
#define DEFAULT_PORT_KEY "default_port"
#define FULL_SCREEN_KEY "full_screen"
#define WINDOW_RESOLUTION_KEY "resolution"
#define VOLUME_KEY "volume"

// Initialize all options to their default values
std::string Options::DEFAULT_ADDRESS = "127.0.0.1";
unsigned int Options::DEFAULT_PORT = 2773;
bool Options::FULL_SCREEN = false;
glm::ivec2 Options::WINDOW_RESOLUTION = glm::ivec2(800, 600);
int Options::VOLUME = 100;

bool Options::LoadOptions() {
	MWLOG(Info, Options, "Loading options");
	std::ifstream optionsFile(OPTIONS_FILE_NAME);
	if (!optionsFile.good()) {
		return false;
	}
	std::string line;
	unsigned int count = 1;
	while (std::getline(optionsFile, line)) {
		std::vector<std::string> parts;
		if (!StringUtils::Split(line, ": ", parts)) {
			continue;
		}
		else if (parts.size() != 2) {
			continue;
		}

		if (parts[0] == DEFAULT_ADDRESS_KEY) {
			DEFAULT_ADDRESS = parts[1];
		}
		else if (parts[0] == DEFAULT_PORT_KEY) {
			DEFAULT_PORT = (unsigned int)std::stoi(parts[1]);
		}
		else if (parts[0] == FULL_SCREEN_KEY) {
			FULL_SCREEN = std::stoi(parts[1]);
		}
		else if (parts[0] == WINDOW_RESOLUTION_KEY) {
			std::vector<std::string> resolutionSplit;
			if (!StringUtils::Split(parts[1], "x", resolutionSplit)) {
				continue;
			}
			if (resolutionSplit.size() != 2) {
				continue;
			}
			WINDOW_RESOLUTION.x = std::stoi(resolutionSplit[0]);
			WINDOW_RESOLUTION.y = std::stoi(resolutionSplit[1]);
		}
		else if (parts[0] == VOLUME_KEY) {
			VOLUME = std::stoi(parts[1]);
		}
	}
	optionsFile.close();
	return true;
}

bool Options::SaveOptions() {
	std::ofstream optionsFile(OPTIONS_FILE_NAME);
	if (!optionsFile.is_open()) {
		return false;
	}
	optionsFile << DEFAULT_ADDRESS_KEY << ": " << DEFAULT_ADDRESS << std::endl;
	optionsFile << DEFAULT_PORT_KEY << ": " << DEFAULT_PORT << std::endl;
	optionsFile << FULL_SCREEN_KEY << ": " << FULL_SCREEN << std::endl;
	optionsFile << WINDOW_RESOLUTION_KEY << ": " << WINDOW_RESOLUTION.x
		<< "x" << WINDOW_RESOLUTION.y << std::endl;
	optionsFile << VOLUME_KEY << ": " << VOLUME << std::endl;
	optionsFile.close();

	return true;
}

void OptionsScene::init() {
	// Initialize the camera
	m_UICamera.init();
	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);

	// Initialize text and sprite shaders
	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);
	m_textShader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_UICamera);

	// Initialize UI groups
	m_mainUIGroup.init(this, MAIN_UI_GROUP, MW::RESOURCES.getFont(
			"Assets/font/arial.ttf"), &m_spriteShader, &m_textShader,
		"textColor");

	// Values for initializing UI
	glm::vec2 winDims = glm::vec2(800, 600);
	glm::vec2 boxDims = glm::vec2(200.0f / winDims.x, 30.0f / winDims.y);
	float cursorWidth = 1.0f;
	float margin = 3.0f / 800.0f;
	float buffer = 0.05f;
	float textScale = 0.25f * ((float)MW::WINDOW.getDimensions().y / winDims.y);
	glm::vec3 textColor = glm::vec3(0.75f, 0.75f, 0.75f);
	Texture* buttonTexture = MW::RESOURCES.getTexture(
		"Assets/texture/button.png");
	Texture* textBoxTexture = MW::RESOURCES.getTexture(
		"Assets/texture/text_box.png");
	Texture* cursorTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cursor.png");
	Texture* switchTexture = MW::RESOURCES.getTexture(
		"Assets/texture/switch.png");
	Texture* cycleTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cycle.png");
	Texture* arrowTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cycle_arrow.png");
	Texture* sliderTexture = MW::RESOURCES.getTexture(
		"Assets/texture/slider.png");

	// Initialize options UI group components
	m_addressBox.init("Default Address", "", glm::vec3(buffer,
		1.0f - (boxDims.y + buffer), 0.0f), boxDims, cursorWidth, margin,
		textScale, textColor, Justification::LEFT, Justification::CENTER,
		textBoxTexture, cursorTexture, 100);
	m_portBox.init("Default Port", "", glm::vec3(buffer + boxDims.x + buffer,
		1.0f - (boxDims.y + buffer), 0.0f), boxDims, cursorWidth, margin,
		textScale, textColor, Justification::LEFT, Justification::CENTER,
		textBoxTexture, cursorTexture, 6);
	m_fullScreenSwitch.init("Fullscreen", "", glm::vec3(buffer,
		1.0f - 2 * (boxDims.y + buffer), 0.0f), boxDims, textScale,
		textColor, Justification::LEFT, Justification::CENTER, switchTexture,
		false);
	m_resolutionCycle.init("Window Resolution",
		{ "800x600", "1080x720", "1920x1080" },
		glm::vec3(buffer + boxDims.x + buffer,
			1.0f - 2 * (boxDims.y + buffer), 0.0f), boxDims, 20.0f, textScale,
		textColor, Justification::LEFT, Justification::CENTER, cycleTexture,
		arrowTexture, 0);
	m_volumeSlider.init("Volume", glm::vec3(buffer,
		1.0f - 3 * (boxDims.y + buffer), 0.0f), boxDims, cursorWidth,
		10.0f / 800.0f,
		textScale, textColor, Justification::LEFT, Justification::CENTER,
		sliderTexture, cursorTexture, 0, 0, 100);
	m_mainUIGroup.addComponents({ &m_addressBox, &m_portBox,
		&m_fullScreenSwitch, &m_resolutionCycle, &m_volumeSlider });

	// Initialize the main UI group components
	m_backButton.init("Back", glm::vec3(0.0f, 0.0f, 0.0f), boxDims,
		textScale, textColor, Justification::CENTER, Justification::CENTER,
		buttonTexture);
	m_defaultsButton.init("Defaults", glm::vec3(0.5f - boxDims.x / 2,
		0.0f, 0.0f), boxDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_saveButton.init("Save", glm::vec3(1.0f - boxDims.x, 0.0f, 0.0f),
		boxDims, textScale, textColor, Justification::CENTER,
		Justification::CENTER, buttonTexture);
	m_mainUIGroup.addComponents({ &m_backButton, &m_defaultsButton,
		&m_saveButton });

	m_initialized = true;
}

void OptionsScene::enter() {
	m_addressBox.setText(Options::DEFAULT_ADDRESS);
	m_portBox.setText(std::to_string(Options::DEFAULT_PORT));
	m_fullScreenSwitch.setText(Options::FULL_SCREEN ? "Enabled" : "Disabled");
	m_fullScreenSwitch.setOn(Options::FULL_SCREEN);
	switch (Options::WINDOW_RESOLUTION.x) {
	case 800: {
		m_resolutionCycle.setSelection(0);
		break;
	}
	case 1080: {
		m_resolutionCycle.setSelection(1);
		break;
	}
	case 1920: {
		m_resolutionCycle.setSelection(2);
		break;
	}
	}
	m_volumeSlider.setValue(Options::VOLUME);

	// Set game controller directions for UI components
	if (MW::INPUT.getGamepadCount() > 0) {
		m_addressBox.setDirections(nullptr, &m_fullScreenSwitch, nullptr,
			&m_portBox);
		m_portBox.setDirections(nullptr, &m_resolutionCycle, &m_addressBox,
			&m_fullScreenSwitch);
		m_fullScreenSwitch.setDirections(&m_addressBox, &m_volumeSlider,
			&m_portBox, &m_resolutionCycle);
		m_resolutionCycle.setDirections(&m_portBox, &m_saveButton,
			&m_fullScreenSwitch, &m_volumeSlider);
		m_volumeSlider.setDirections(&m_fullScreenSwitch, &m_backButton,
			&m_resolutionCycle, nullptr);
		m_backButton.setDirections(&m_volumeSlider, nullptr, nullptr,
			&m_defaultsButton);
		m_defaultsButton.setDirections(&m_volumeSlider, nullptr, &m_backButton,
			&m_saveButton);
		m_saveButton.setDirections(&m_resolutionCycle, nullptr,
			&m_defaultsButton, nullptr);
		m_mainUIGroup.setSelectedComponent(&m_backButton);
	}
	m_mainUIGroup.setEnabled(true);
}

void OptionsScene::draw() {
	m_mainUIGroup.draw();
}

void OptionsScene::processInput() {
	m_mainUIGroup.processInput();
}

void OptionsScene::componentEvent(unsigned int groupID,
	unsigned int componentID, unsigned int eventID) {
	switch (groupID) {
	case MAIN_UI_GROUP: {
		if (componentID == m_fullScreenSwitch.getID()) {
			m_fullScreenSwitch.setText(m_fullScreenSwitch.isOn()
				? "Enabled" : "Disabled");
		}
		else if (componentID == m_backButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				MW::SetScene(m_returnScene);
			}
		}
		else if (componentID == m_defaultsButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				m_addressBox.setText("127.0.0.1");
				m_portBox.setText(std::to_string(2773));
				m_fullScreenSwitch.setText("Disabled");
				m_fullScreenSwitch.setOn(false);
				m_resolutionCycle.setSelection(0);
				m_volumeSlider.setValue(100);
			}
		}
		else if (componentID == m_saveButton.getID()) {
			if (eventID == UI::Button::CLICKED_EVENT) {
				// Set the options and save them to the disk
				Options::DEFAULT_ADDRESS = m_addressBox.getText();
				Options::DEFAULT_PORT = std::atoi(m_portBox.getText().c_str());
				Options::FULL_SCREEN = m_fullScreenSwitch.isOn();
				switch (m_resolutionCycle.getSelection()) {
				case 0: {
					Options::WINDOW_RESOLUTION = glm::ivec2(800, 600);
					break;
				}
				case 1: {
					Options::WINDOW_RESOLUTION = glm::ivec2(1080, 720);
					break;
				}
				case 2: {
					Options::WINDOW_RESOLUTION = glm::ivec2(1920, 1080);
					break;
				}
				}
				Options::VOLUME = m_volumeSlider.getValue();
				Options::SaveOptions();

				// Apply the options to the current runtime
				MW::WINDOW.setDimensions(Options::WINDOW_RESOLUTION);
				MW::WINDOW.setFullScreen(Options::FULL_SCREEN);
				MW::AUDIO.setGain((float)Options::VOLUME / 100.0f);
			}
		}
		break;
	}
	}
}

void OptionsScene::updateWindowSize() {
	m_mainUIGroup.updateWindowSize();
	m_UICamera.position = glm::vec3(MW::WINDOW.getDimensions().x / 2.0f,
		MW::WINDOW.getDimensions().y / 2.0f, 0.0f);
}

void OptionsScene::update(float deltaTime) {
	m_UICamera.update(deltaTime);
	m_mainUIGroup.update(deltaTime);
}

void OptionsScene::exit() {
	m_mainUIGroup.setEnabled(false);
}

void OptionsScene::destroy() {
	m_mainUIGroup.destroy();

	m_initialized = false;
}