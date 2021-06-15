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

	m_sprite.init(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(50.0f, 50.0f),
		MW::RESOURCES.getTexture("Assets/texture/sprite.png"));
}

void TestScene::enter() {
	m_UIGroup.setEnabled(true);
	MW::INPUT.addInputListener(this);
}

void TestScene::draw() {
	MW::RENDERER.submit({ &m_sprite }, &m_spriteShader);

	m_UIGroup.draw();
}

void TestScene::processInput() {
	if (MW::INPUT.isKeyPressed(F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}

	m_UIGroup.processInput();
}

void TestScene::keyPressed(int key) {
	MWLOG(Info, TestScene, "Key ", key, " pressed");
}

void TestScene::keyReleased(int key) {
	MWLOG(Info, TestScene, "Key ", key, " released");
}

void TestScene::textTyped(char text) {
	MWLOG(Info, TestScene, "Character \"", text, "\" typed");
}

void TestScene::buttonPressed(int button) {
	MWLOG(Info, TestScene, "Mouse button ", button, " pressed");
}

void TestScene::buttonReleased(int button) {
	MWLOG(Info, TestScene, "Mouse button ", button, " released");
}

void TestScene::cursorMoved() {
	glm::vec2 mousePos = MW::INPUT.getCursorPosition(&m_camera);
	
}

void TestScene::scrolled(const glm::vec2& distance) {
	MWLOG(Info, TestScene, "Mouse scrolled (", distance.x, ", ", distance.y,
		")");
}

void TestScene::gamepadConnected(int gp) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " connected");
}

void TestScene::gamepadDisconnected(int gp) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " disconnected");
}

void TestScene::gamepadButtonPressed(int gp, unsigned int button) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " button ", button, " pressed");
}

void TestScene::gamepadButtonReleased(int gp, unsigned int button) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " button ", button, " released");
}

void TestScene::gamepadAxisMoved(int gp, unsigned int axis) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " axis ", axis, " moved");
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
	MW::INPUT.removeInputListener(this);
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