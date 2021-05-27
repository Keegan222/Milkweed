#pragma once

#include <Milkweed/MW.h>

using namespace Milkweed;

class TestScene : public Scene {
public:
	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
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
	UI::TextArea m_textArea;
};

class MWTest {
public:
	static TestScene TEST_SCENE;
};