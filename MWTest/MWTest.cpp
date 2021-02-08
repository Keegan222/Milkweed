#include <Milkweed/Audio.h>

#include "MWTest.h"

void TestScene::init() {
	MW::App::AUDIO.playMusic(
		MW::App::RESOURCES.getSound("Assets/audio/music.wav"));

	m_shader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		MW::Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix");

	m_sprite.init(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::ivec2(128.0f, 128.0f),
		MW::App::RESOURCES.getTexture("Assets/texture/image.png"),
		glm::ivec2(4, 2), 30.0f);

}

void TestScene::enter() {

}

void TestScene::draw() {
	MW::App::RENDERER.submit({ &m_sprite }, &m_shader);
}

void TestScene::processInput() {
	if (MW::App::INPUT.isButtonPressed(MW::Button::B_LEFT)) {
		MW::App::AUDIO.playSound(
			MW::App::RESOURCES.getSound("Assets/audio/sound.wav"));
	}

	if (MW::App::INPUT.isKeyPressed(MW::Key::K_R)) {
		MW::App::WINDOW.setFullScreen(true);
	}
	
	if (MW::App::INPUT.isKeyPressed(MW::Key::K_W)) {
		MW::App::WINDOW.setFullScreen(false);
	}

	if (MW::App::INPUT.isKeyPressed(MW::Key::K_X)) {
		MW::App::WINDOW.setDimensions(glm::ivec2(800, 600));
	}

	if (MW::App::INPUT.isKeyPressed(MW::Key::K_C)) {
		MW::App::WINDOW.setDimensions(glm::ivec2(400, 300));
	}
}

void TestScene::update(float deltaTime) {
	m_shader.getCamera()->update(deltaTime);

	m_sprite.update(deltaTime);
}

void TestScene::exit() {

}

void TestScene::destroy() {

}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::App::Init("MWTest", glm::ivec2(800, 600), false,
		60.0f, { &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}