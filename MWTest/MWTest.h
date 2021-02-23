#pragma once

#include <Milkweed/MW.h>

class TestScene : public MW::Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

private:
	MW::Sound* m_music = nullptr;
	MW::Sound* m_sound = nullptr;
	MW::Shader m_shader;
	MW::Font* m_font = nullptr;
	float m_width = 0.0f;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};