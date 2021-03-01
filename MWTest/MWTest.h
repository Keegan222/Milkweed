#pragma once

#include <Milkweed/MW.h>

using namespace Milkweed;

class TestScene : public Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	Sound* m_music = nullptr;
	Sound* m_sound = nullptr;
	Shader m_shader;
	Font* m_font = nullptr;
	UIGroup m_UIGroup;
	UILabel m_label;
	UILabel m_label1;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};