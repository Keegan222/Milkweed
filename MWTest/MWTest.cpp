#include <Milkweed/Audio.h>

#include "MWTest.h"

using namespace Milkweed;

void TestScene::init() {
	m_music = MW::RESOURCES.getSound("Assets/audio/music.wav");

	m_sound = MW::RESOURCES.getSound("Assets/audio/sound.wav");

	m_shader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix");
	m_font = MW::RESOURCES.getFont("Assets/font/arial.ttf");

	m_shader.getCamera()->position = glm::vec3(
		MW::WINDOW.getDimensions().x / 2,
		MW::WINDOW.getDimensions().y / 2, 0.0f);

	m_UIGroup.init(this, m_font, &m_shader, "textColor");
	m_label.init("Hello World!", glm::vec3(0.0f), glm::vec2(800.0f, 100.0f),
		1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	m_UIGroup.addComponent(&m_label);
	m_label1.init("Goodbye World!", glm::vec3(0.0f), glm::vec2(800.0f, 100.0f),
		1.0f, glm::vec3(0.0f, 0.0f, 1.0f), Justification::RIGHT);
	m_UIGroup.addComponent(&m_label1);
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_UIGroup.draw();
}

void TestScene::processInput() {
	m_UIGroup.processInput();
}

void TestScene::update(float deltaTime) {
	m_shader.getCamera()->update(deltaTime);
	m_UIGroup.update(deltaTime);
}

void TestScene::exit() {

}

void TestScene::destroy() {
	m_UIGroup.destroy();

}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::Init("MWTest", glm::ivec2(800, 600), false, 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}