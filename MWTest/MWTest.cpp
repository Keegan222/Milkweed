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
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_shader.upload3fVector("textColor", glm::vec3(1.0f, 0.0f, 1.0f));
	MW::RENDERER.submit("Hello World", glm::vec3(0.0f),
		glm::vec4(0.0f, 0.0f, m_width, 100.0f), 1.0f,
		glm::vec3(1.0f, 0.0f, 1.0f), m_font, &m_shader);
}

void TestScene::processInput() {
	if (MW::INPUT.isKeyPressed(Key::K_R)) {
		if (MW::WINDOW.isFullScreen()) {
			MW::WINDOW.setFullScreen(false);
		}
		else {
			MW::WINDOW.setFullScreen(true);
		}
	}

	if (MW::INPUT.isKeyPressed(Key::K_W)) {
		MW::RENDERER.dumpNextFrame();
	}
}

void TestScene::update(float deltaTime) {
	m_shader.getCamera()->update(deltaTime);
	m_width += 0.4f * deltaTime;
}

void TestScene::exit() {

}

void TestScene::destroy() {

}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::Init("MWTest", glm::ivec2(800, 600), false, 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}