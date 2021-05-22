/*
* File:		OptionsScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#ifndef OPTIONS_SCENE_H
#define OPTIONS_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

class Options {
public:
	static std::string DEFAULT_ADDRESS;
	static unsigned int DEFAULT_PORT;
	static bool FULL_SCREEN;
	static glm::ivec2 WINDOW_RESOLUTION;
	static int VOLUME;

	static bool LoadOptions();
	static bool SaveOptions();
};

class OptionsScene : public Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void componentEvent(unsigned int groupID, unsigned int componentID,
		unsigned int eventID) override;
	void updateWindowSize() override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;
	void setReturnScene(Scene* returnScene) { m_returnScene = returnScene; }

private:
	const static unsigned int OPTIONS_UI_GROUP = 0;
	const static unsigned int MAIN_UI_GROUP = 1;
	Scene* m_returnScene = nullptr;
	Camera m_UICamera;
	Shader m_spriteShader, m_textShader;
	UI::UIGroup m_optionsUIGroup, m_mainUIGroup;
	UI::TextBox m_addressBox, m_portBox;
	UI::Switch m_fullScreenSwitch;
	UI::Cycle m_resolutionCycle;
	UI::Slider m_volumeSlider;
	UI::Button m_backButton, m_defaultsButton, m_saveButton;
};

#endif