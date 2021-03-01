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
	Justification m_textJust = Justification::LEFT;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};