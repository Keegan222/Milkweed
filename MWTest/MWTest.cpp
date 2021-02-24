#include <Milkweed/Audio.h>

#include "MWTest.h"

void TestScene::init() {
	m_music = MW::App::RESOURCES.getSound("Assets/audio/music.wav");
	// MW::App::AUDIO.playMusic(m_music);

	m_sound = MW::App::RESOURCES.getSound("Assets/audio/sound.wav");

	m_shader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		MW::Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix");
	m_font = MW::App::RESOURCES.getFont("Assets/font/arial.ttf");
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_shader.upload3fVector("textColor", glm::vec3(1.0f, 0.0f, 1.0f));
	MW::App::RENDERER.submit("Hello World", glm::vec3(0.0f),
		glm::vec4(0.0f, 0.0f, m_width, 100.0f), 1.0f,
		glm::vec3(1.0f, 0.0f, 1.0f), m_font, &m_shader);
}

void TestScene::processInput() {
	if (MW::App::INPUT.isKeyPressed(MW::Key::K_R)) {
		if (MW::App::WINDOW.isFullScreen()) {
			MW::App::WINDOW.setFullScreen(false);
		}
		else {
			MW::App::WINDOW.setFullScreen(true);
		}
	}

	if (MW::App::INPUT.isKeyPressed(MW::Key::K_W)) {
		MW::App::RENDERER.dumpNextFrame();
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
	MW::App::Init("MWTest", glm::ivec2(800, 600), false, 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}