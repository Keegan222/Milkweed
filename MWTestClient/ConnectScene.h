/*
* File:		ConnectScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.22
*/

#ifndef CONNECT_SCENE_H
#define CONNECT_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

class ConnectScene : public Scene {
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
	UI::TextBox m_usernameBox, m_addressBox, m_portBox;
	UI::Button m_backButton, m_defaultsButton, m_connectButton;

	void setComponentDirections();
};

#endif