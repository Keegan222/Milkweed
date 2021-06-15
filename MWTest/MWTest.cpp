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
}

void TestScene::enter() {
	m_UIGroup.setEnabled(true);
	MW::INPUT.addInputListener(this);
}

void TestScene::draw() {
	m_UIGroup.draw();
}

void TestScene::processInput() {
	if (MW::INPUT.isKeyPressed(F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}

	// Test gamepad input
	for (unsigned int gp = 0; gp < MW::INPUT.getGamepadCount(); gp++) {
		if (MW::INPUT.isGamepadButtonPressed(gp, GamepadButton::G_CIRCLE)) {
			MWLOG(Info, TestScene, gp, ": CIRCLE");
		}
	}

	m_UIGroup.processInput();
}

void TestScene::gamepadButtonPressed(int gp, unsigned int button) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " button press ", button);
}

void TestScene::gamepadButtonReleased(int gp, unsigned int button) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " button release ", button);
}

void TestScene::gamepadAxisMoved(int gp, unsigned int axis) {
	MWLOG(Info, TestScene, "Gamepad ", gp, " axis moved ", axis);
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