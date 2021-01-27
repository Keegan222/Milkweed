#include <Milkweed/AudioManager.h>

#include "MWTest.h"

void TestScene::init() {
	m_music = MW::App::RESOURCES.getSound("Assets/audio/music.wav");
	// MW::App::AUDIO.playMusic(m_music);

	m_sound = MW::App::RESOURCES.getSound("Assets/audio/sound.wav");
	
	m_text = MW::Label("Hello World!", glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 100.0f, 100.0f), 1.0f,
		glm::vec3(1.0f, 0.0f, 0.0f));
	m_shader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		MW::Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix");
	m_font = MW::App::RESOURCES.getFont("Assets/font/arial.ttf");
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_shader.upload3fVector("textColor", m_text.color);
	MW::App::RENDERER.submit(m_text, m_font, &m_shader);
}

void TestScene::processInput() {

}

void TestScene::update(float deltaTime) {
	m_shader.getCamera()->update(deltaTime);
	m_text.bounds.z += 0.1f * deltaTime;
}

void TestScene::exit() {

}

void TestScene::destroy() {

}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::App::Init(MW::WindowAttributes(glm::ivec2(800, 600), "MWTest"), 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}