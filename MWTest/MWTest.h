#pragma once

#include <Milkweed/MW.h>

using namespace Milkweed;

class TestScene : public Scene, InputListener {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void textTyped(char text) override;
	void componentEvent(unsigned int groupID,
		unsigned int componentID, unsigned int eventID) override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	Sound* m_music = nullptr;
	Sound* m_sound = nullptr;
	Camera m_camera;
	Shader m_spriteShader;
	Shader m_textShader;
	Font* m_font = nullptr;
	UIGroup m_UIGroup;
	UIButton m_button;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};