#include <Milkweed/Audio.h>

#include "MWTest.h"

using namespace Milkweed;

void TestScene::init() {
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
	m_button.init("A test of some stuff!! A", glm::vec3(0.0f), glm::vec2(300.0f, 75.0f),
		0.25f, glm::vec3(1.0f, 0.0f, 0.0f), Justification::CENTER,
		Justification::CENTER, MW::RESOURCES.getTexture("Assets/texture/button.png"));
	m_UIGroup.addComponent(&m_button);
	m_switch.init("Switch", "On/off", glm::vec3(400.0f, 100.0f, 0.0f),
		glm::vec2(300.0f, 40.0f), 0.3f, glm::vec3(1.0f, 1.0f, 0.0f),
		Justification::LEFT, Justification::CENTER,
		MW::RESOURCES.getTexture("Assets/texture/switch.png"), false);
	m_UIGroup.addComponent(&m_switch);
	m_textBox.init("This is a text box", "Boxtest", glm::vec3(400.0f, 0.0f, 0.0f),
		glm::vec2(300.0f, 40.0f), 0.3f, glm::vec3(1.0f, 1.0f, 1.0f),
		Justification::LEFT, Justification::CENTER,
		MW::RESOURCES.getTexture("Assets/texture/text_box.png"),
		MW::RESOURCES.getTexture("Assets/texture/cursor.png"), 100);
	m_UIGroup.addComponent(&m_textBox);
	m_slider.init("Slider", glm::vec3(400.0f, 300.0f, 0.0f),
		glm::vec2(200.0f, 40.0f), 0.3f, glm::vec3(1.0f, 1.0f, 1.0f),
		Justification::LEFT, Justification::CENTER,
		MW::RESOURCES.getTexture("Assets/texture/slider.png"),
		MW::RESOURCES.getTexture("Assets/texture/cursor.png"), 1, 3, 4);
	m_UIGroup.addComponent(&m_slider);
	m_cycle.init("Test", { "Option 1", "Option 2", "Option 3" },
		glm::vec3(400.0f, 370.0f, 0.0f), glm::vec2(200.0f, 40.0f), 20.0f,
		0.3f, glm::vec3(1.0f, 1.0f, 1.0f), Justification::CENTER,
		Justification::CENTER, MW::RESOURCES.getTexture(
			"Assets/texture/cycle.png"),
		MW::RESOURCES.getTexture("Assets/texture/cycle_arrow.png"), 0);
	m_UIGroup.addComponent(&m_cycle);

	m_ASprite.init(glm::vec3(100.0f, 100.0f, 0.0f), glm::vec2(100.0f, 100.0f),
		MW::RESOURCES.getTexture("Assets/texture/image.png"),
		glm::ivec2(4, 2), 30.0f);
	m_ASprite.rotation = -180;

	MW::INPUT.addInputListener(this);
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_UIGroup.draw();
	MW::RENDERER.submit({ &m_ASprite }, &m_spriteShader);
}

void TestScene::processInput() {
	if (MW::INPUT.isKeyPressed(F_11)) {
		MW::WINDOW.setFullScreen(!MW::WINDOW.isFullScreen());
	}
	if (MW::INPUT.isButtonPressed(B_LEFT)) {
		m_textBox.setPosition(glm::vec3(400.0f, 200.0f, 0.0f));
	}
	m_UIGroup.processInput();
}

void TestScene::updateWindowSize() {
	m_camera.destroy();
	m_camera.init();
	m_camera.position = glm::vec3(
		MW::WINDOW.getDimensions().x / 2,
		MW::WINDOW.getDimensions().y / 2, 0.0f);
}

void TestScene::textTyped(char text) {

}

void TestScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {
	MWLOG(Info, MWTest, "Component event from group ", groupID, " on component ",
		componentID, " with event ", eventID);

	if (groupID == m_UIGroup.getID()) {
		if (componentID == m_button.getID()) {

		}
		else if (componentID == m_switch.getID()) {
			if (eventID == UI::Switch::ON_EVENT) {
				MWLOG(Info, MWTest, "On event");
			}
			else if (eventID == UI::Switch::OFF_EVENT) {
				MWLOG(Info, MWTest, "Off event");
			}
		}
		else if (componentID == m_slider.getID()) {
			if (eventID == UI::Slider::SELECTED_EVENT) {
				MWLOG(Info, MWTest, "Slider selected");
			}
			else if (eventID == UI::Slider::VALUE_UPDATE_EVENT) {
				MWLOG(Info, MWTest, "Slider value set");
			}
		}
	}
}

void TestScene::update(float deltaTime) {
	m_spriteShader.getCamera()->update(deltaTime);
	m_UIGroup.update(deltaTime);
	m_timer += deltaTime;
	if (m_timer >= 5.0f) {
		m_ASprite.rotation++;
		m_timer = 0.0f;
	}

	m_ASprite.update(deltaTime);
}

void TestScene::exit() {

}

void TestScene::destroy() {
	MW::INPUT.removeInputListener(this);
	m_UIGroup.destroy();
	m_ASprite.destroy();
}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::Init("MWTest", glm::ivec2(800, 600), false, 60.0f, 1.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}