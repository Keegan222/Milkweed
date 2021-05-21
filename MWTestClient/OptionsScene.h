/*
* File:		OptionsScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#ifndef OPTIONS_SCENE_H
#define OPTIONS_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

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

private:
	const static unsigned int OPTIONS_UI_GROUP = 0;
	const static unsigned int MAIN_UI_GROUP = 1;
	Camera m_UICamera;
	Shader m_spriteShader, m_textShader;
	UI::UIGroup m_optionsUIGroup, m_mainUIGroup;
	UI::TextLabel m_addressLabel, m_portLabel, m_fullScreenLabel,
		m_resolutionLabel, m_volumeLabel;
	UI::TextBox m_addressBox, m_portBox;
	/*UI::Switch m_fullScreenSwitch;
	UI::Cycle m_resolutionCycle;
	UI::Slider m_volumeSlider;*/
	UI::Button m_backButton, m_defaultsButton, m_saveButton;
};

#endif