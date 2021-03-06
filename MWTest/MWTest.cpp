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

	m_UIGroup.init(this, 0, m_font, &m_shader, "textColor");
	m_label.init("", glm::vec3(0.0f), glm::vec2(800.0f, 600.0f),
		1.0f, glm::vec3(1.0f, 0.0f, 0.0f), Justification::LEFT,
		Justification::TOP, true);
	m_UIGroup.addComponent(&m_label);

	MW::INPUT.addInputListener(this);
}

void TestScene::enter() {

}

void TestScene::draw() {
	m_UIGroup.draw();
}

void TestScene::processInput() {
	m_UIGroup.processInput();

	if (MW::INPUT.isKeyPressed(F_BACKSPACE)) {
		if (!m_label.getText().empty()) {
			m_label.setText(m_label.getText().substr(0,
				m_label.getText().length() - 1));
		}
	}

	if (MW::INPUT.isKeyPressed(F_LEFT)) {
		if (MW::INPUT.isKeyDown(F_LEFT_SHIFT)) {
			m_label.setVJustification(Justification::TOP);
		}
		else {
			m_label.setHJustification(Justification::LEFT);
		}
	}
	else if (MW::INPUT.isKeyPressed(F_UP)) {
		if (MW::INPUT.isKeyDown(F_LEFT_SHIFT)) {
			m_label.setVJustification(Justification::CENTER);
		}
		else {
			m_label.setHJustification(Justification::CENTER);
		}
	}
	else if (MW::INPUT.isKeyPressed(F_RIGHT)) {
		if (MW::INPUT.isKeyDown(F_LEFT_SHIFT)) {
			m_label.setVJustification(Justification::BOTTOM);
		}
		else {
			m_label.setHJustification(Justification::RIGHT);
		}
	}
}

void TestScene::textTyped(char text) {
	m_label.setText(m_label.getText() + std::string(1, text));
}

void TestScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {

}

void TestScene::update(float deltaTime) {
	m_shader.getCamera()->update(deltaTime);
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