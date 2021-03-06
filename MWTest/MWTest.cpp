#include <Milkweed/Audio.h>

#include "MWTest.h"

using namespace Milkweed;

void TestScene::init() {
	m_music = MW::RESOURCES.getSound("Assets/audio/music.wav");

	m_sound = MW::RESOURCES.getSound("Assets/audio/sound.wav");

	m_camera.init();
	m_camera.position = glm::vec3(
		MW::WINDOW.getDimensions().x / 2,
		MW::WINDOW.getDimensions().y / 2, 0.0f);

	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_camera);
	m_textShader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_camera);

	m_font = MW::RESOURCES.getFont("Assets/font/arial.ttf");

	m_UIGroup.init(this, 0, m_font, &m_spriteShader, &m_textShader, "textColor");
	m_button.init("Click Me", glm::vec3(0.0f), glm::vec2(300.0f, 75.0f),
		0.25f, glm::vec3(1.0f, 0.0f, 0.0f), Justification::CENTER,
		Justification::CENTER, MW::RESOURCES.getTexture("Assets/texture/button.png"));
	m_UIGroup.addComponent(&m_button);

	MW::INPUT.addInputListener(this);
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_UIGroup.draw();
}

void TestScene::processInput() {
	m_UIGroup.processInput();
}

void TestScene::textTyped(char text) {

}

void TestScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	MWLOG(Info, MWTest, "Component event from group ", groupID, " on component ",
		componentID, " with event ", eventID);
}

void TestScene::update(float deltaTime) {
	m_spriteShader.getCamera()->update(deltaTime);
	m_UIGroup.update(deltaTime);
}

void TestScene::exit() {

}

void TestScene::destroy() {
	MW::INPUT.removeInputListener(this);
	m_UIGroup.destroy();
}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::Init("MWTest", glm::ivec2(800, 600), false, 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}