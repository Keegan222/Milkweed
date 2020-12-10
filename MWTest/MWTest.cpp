#include <iostream>
#include <cstdlib>
#include <time.h>

#include "MWTest.h"

TestScene1 MWTest::TEST_SCENE_1;

int TestScene1::randBetween(int a, int b) {
	return (rand() % (b - a)) + a;
}

void TestScene1::init() {
	MW::App::Log("Init scene 1");

	srand((unsigned int)time(0));

	m_shader.init(
		"Assets/Shaders/vertex_shader.glsl",
		"Assets/Shaders/fragment_shader.glsl",
		{
			MW::VertexAttribute("inPosition", 2, GL_FLOAT, GL_FALSE,
				4 * sizeof(float), 0),
			MW::VertexAttribute("inTextureCoords", 2, GL_FLOAT, GL_FALSE,
				4 * sizeof(float), 2 * sizeof(float)),
		},
		"cameraMatrix"
	);
}

void TestScene1::enter() {
	MW::App::Log("Enter scene 1");
}

void TestScene1::draw() {
	m_frames++;

	m_spritePointers.resize(m_sprites.size());
	for (unsigned int i = 0; i < m_sprites.size(); i++) {
		m_spritePointers[i] = &m_sprites[i];
	}
	MW::App::RENDERER.submit(m_spritePointers, &m_shader);
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
			+ std::to_string(m_frames) + " frames, "
			+ std::to_string(m_sprites.size()) + " sprites");
		m_timer = 0.0f;
		m_frames = 0;
	}

	m_sprites.push_back(MW::Sprite(
		glm::vec2(randBetween(0, 800), randBetween(0, 600)),
		glm::vec2(100.0f, 100.0f), MW::App::RESOURCES.getTexture(
			"Assets/Textures/texture" + std::to_string(randBetween(0, 4))
			+ ".png")));

	m_shader.getCamera()->update(deltaTime);
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