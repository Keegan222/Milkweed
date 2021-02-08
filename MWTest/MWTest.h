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
	MW::AnimatedSprite m_sprite;
	MW::Shader m_shader;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};