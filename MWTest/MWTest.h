#pragma once

#include <Milkweed/MW.h>

class TestScene1 : public MW::Scene {
public:
	void init();
	void enter();
	void draw();
	void processInput();
	void update(float deltaTime);
	void exit();
	void destroy();

private:
	float m_timer = 0.0f;
	double m_startTime = 0.0;
	unsigned int m_frames = 0, m_seconds = 0;

	MW::Shader m_shader;
	MW::AnimatedSprite m_sprite;
};

class MWTest {
public:
	static TestScene1 TEST_SCENE_1;
};