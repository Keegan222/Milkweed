/*
* File:		TitleScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

class TitleScene : public Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void gamepadConnected(int gp) override;
	void gamepadDisconnected(int gp) override;
	void componentEvent(unsigned int groupID, unsigned int componentID,
		unsigned int eventID) override;
	void updateWindowSize() override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	const static unsigned int MAIN_UI_GROUP = 0;
	Camera m_UICamera;
	Shader m_spriteShader, m_textShader;
	UI::UIGroup m_mainUIGroup;
	UI::Button m_connectButton, m_optionsButton, m_quitButton;

	void setComponentDirections();
};

#endif