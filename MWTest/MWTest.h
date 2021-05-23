#pragma once

#include <Milkweed/MW.h>

using namespace Milkweed;

class TestScene : public Scene, InputListener {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void updateWindowSize() override;
	void textTyped(char text) override;
	void componentEvent(unsigned int groupID,
		unsigned int componentID, unsigned int eventID) override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	Camera m_camera;
	Shader m_spriteShader;

	Shader m_textShader;
	Font* m_font = nullptr;
	UI::UIGroup m_UIGroup;
	UI::Button m_button;
	UI::Switch m_switch;
	UI::Slider m_slider;
	UI::TextBox m_textBox;
	UI::Cycle m_cycle;
	AnimatedSprite m_ASprite;
	float m_timer = 0.0f;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};