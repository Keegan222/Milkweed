#include <iostream>

#include "MWTest.h"

TestScene1 MWTest::TEST_SCENE_1;

void TestScene1::init() {
	MW::App::Log("Init scene 1");
}

void TestScene1::enter() {
	MW::App::Log("Enter scene 1");
}

void TestScene1::draw() {
	m_frames++;
}

void TestScene1::processInput() {

}

void TestScene1::update(float deltaTime) {
	m_timer += deltaTime;
	if (glfwGetTime() - m_startTime >= 1.0) {
		m_seconds++;
		m_startTime = glfwGetTime();
		MW::App::Log("Second " + std::to_string(m_seconds)
			+ ": " + std::to_string(m_timer) + " updates, "
			+ std::to_string(m_frames) + " frames");
		m_timer = 0.0f;
		m_frames = 0;
	}
}

void TestScene1::exit() {
	MW::App::Log("Exit scene 1");
}

void TestScene1::destroy() {
	MW::App::Log("Destroy scene 1");
}

int main(int argc, char** argv) {
	MW::App::Init(MW::WindowAttributes(glm::ivec2(800, 600), "MWTest"), 60,
		{ &MWTest::TEST_SCENE_1 }, &MWTest::TEST_SCENE_1);
}