#include <Milkweed/Audio.h>

#include "MWTest.h"

using namespace Milkweed;

void TestScene::init() {
	// Set up the camera and position it
	m_camera.init();
	m_camera.position = glm::vec3(
		MW::WINDOW.getDimensions().x / 2,
		MW::WINDOW.getDimensions().y / 2, 0.0f);

	// Set up sprite and text shading
	m_spriteShader.init("Assets/shader/sprite_vertex_shader.glsl",
		"Assets/shader/sprite_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_camera);
	m_textShader.init("Assets/shader/text_vertex_shader.glsl",
		"Assets/shader/text_fragment_shader.glsl",
		Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix", &m_camera);

	// Prepare to set up the UI
	float textScale = 0.5f;
	glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 cDims = glm::vec2(0.75f, 0.75f);
	Texture* cTexture = MW::RESOURCES.getTexture(
		"Assets/texture/text_area.png");
	Texture* cursorTexture = MW::RESOURCES.getTexture(
		"Assets/texture/cursor.png");

	// Set up the UI
	m_UIGroup.init(this, 0, MW::RESOURCES.getFont("Assets/font/arial.ttf"),
		&m_spriteShader, &m_textShader, "textColor");
	m_textArea.init("", 10, glm::vec3(0.5f - cDims.x / 2.0f,
		0.5f - cDims.y / 2.0f, 0.0f), cDims, 10.0f, textScale, textColor,
		Justification::LEFT, Justification::CENTER, cTexture, cursorTexture);
	m_UIGroup.addComponent(&m_textArea);
	m_textArea.setLineWrapEnabled(true);
	m_textArea.setEditable(true);
	m_textArea.setScrollEnabled(true);
	m_textArea.setEnabled(true);
}

void TestScene::enter() {
	m_UIGroup.setEnabled(true);
}

void TestScene::draw() {
	m_UIGroup.draw();
}

void TestScene::processInput() {
	if (MW::INPUT.isKeyPressed(F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}

	m_UIGroup.processInput();
}

void TestScene::updateWindowSize() {
	m_UIGroup.updateWindowSize();
	m_camera.position = glm::vec3(
		MW::WINDOW.getDimensions().x / 2,
		MW::WINDOW.getDimensions().y / 2, 0.0f);
}

void TestScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	MWLOG(Info, TestScene, "Component event ", eventID, " on component ",
		componentID, " in group ", groupID);
}

void TestScene::update(float deltaTime) {
	m_camera.update(deltaTime);
	m_UIGroup.update(deltaTime);
}

void TestScene::exit() {
	m_UIGroup.setEnabled(false);
}

void TestScene::destroy() {
	m_camera.destroy();
	m_spriteShader.destroy();
	m_textShader.destroy();
	m_UIGroup.destroy();
}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::Init("MWTest", glm::ivec2(800, 600), false, 60.0f, 1.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}