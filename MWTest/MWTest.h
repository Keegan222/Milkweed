#pragma once

#include <Milkweed/MW.h>

using namespace Milkweed;

class TestScene : public Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void textTyped(char text) override;
	void buttonPressed(int button) override;
	void buttonReleased(int button) override;
	void cursorMoved() override;
	void scrolled(const glm::vec2& distance) override;
	void gamepadConnected(int gp) override;
	void gamepadDisconnected(int gp) override;
	void gamepadButtonPressed(int gp, unsigned int button) override;
	void gamepadButtonReleased(int gp, unsigned int button) override;
	void gamepadAxisMoved(int gp, unsigned int axis) override;
	void updateWindowSize() override;
	void componentEvent(unsigned int groupID,
		unsigned int componentID, unsigned int eventID) override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	Camera m_camera;
	Shader m_spriteShader;
	Shader m_textShader;
	UI::UIGroup m_UIGroup;
	Sprite m_sprite;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};