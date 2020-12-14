#include <iostream>
#include <cstdlib>
#include <time.h>

#include "MWTest.h"

TestScene1 MWTest::TEST_SCENE_1;

void TestScene1::init() {
	MW::App::Log("Init scene 1");

	srand((unsigned int)time(0));

	m_shader.init(
		"Assets/Shaders/vertex_shader.glsl",
		"Assets/Shaders/fragment_shader.glsl",
		{
			MW::VertexAttribute("inPosition", 3, GL_FLOAT, GL_FALSE,
				5 * sizeof(float), 0),
			MW::VertexAttribute("inTextureCoords", 2, GL_FLOAT, GL_FALSE,
				5 * sizeof(float), 3 * sizeof(float)),
		},
		"cameraMatrix"
	);

	m_sprite = MW::AnimatedSprite(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec2(100.0f, 100.0f), MW::App::RESOURCES.getTexture(
			"Assets/Textures/sheet.png"), glm::ivec2(3, 2), 60.0f);
	m_sprite1 = MW::Sprite(glm::vec3(100.0f, 100.0f, 0.0f),
		glm::vec2(100.0f, 100.0f), MW::App::RESOURCES.getTexture(
			"Assets/Textures/texture0.png"));
}

void TestScene1::enter() {
	MW::App::Log("Enter scene 1");
}

void TestScene1::draw() {
	m_frames++;

	MW::App::RENDERER.submit({ &m_sprite, &m_sprite1 }, &m_shader);
}

void TestScene1::processInput() {
	if (MW::App::INPUT.isKeyDown(GLFW_KEY_A)) {
		m_shader.getCamera()->velocity.x = -5.0f;
	}
	else if (MW::App::INPUT.isKeyDown(GLFW_KEY_D)) {
		m_shader.getCamera()->velocity.x = 5.0f;
	}
	else {
		m_shader.getCamera()->velocity.x = 0.0f;
	}

	if (MW::App::INPUT.isKeyDown(GLFW_KEY_S)) {
		m_shader.getCamera()->velocity.y = -5.0f;
	}
	else if (MW::App::INPUT.isKeyDown(GLFW_KEY_W)) {
		m_shader.getCamera()->velocity.y = 5.0f;
	}
	else {
		m_shader.getCamera()->velocity.y = 0.0f;
	}

	if (MW::App::INPUT.isKeyDown(GLFW_KEY_Q)) {
		m_shader.getCamera()->scaleVelocity = -0.01f;
	}
	else if (MW::App::INPUT.isKeyDown(GLFW_KEY_E)) {
		m_shader.getCamera()->scaleVelocity = 0.01f;
	}
	else {
		m_shader.getCamera()->scaleVelocity = 0.0f;
	}

	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_SPACE)) {
		if (m_sprite.isPlaying()) {
			m_sprite.pause();
		}
		else {
			m_sprite.play();
		}
	}

	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_P)) {
		m_sprite.stop();
	}

	MW::App::Log(std::to_string(m_sprite.flipVertical));

	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_RIGHT)) {
		m_sprite.flipHorizontal = !m_sprite.flipHorizontal;
	}
	else if (MW::App::INPUT.isKeyPressed(GLFW_KEY_UP)) {
		m_sprite.flipVertical = !m_sprite.flipVertical;
	}
}

void TestScene1::update(float deltaTime) {
	m_timer += deltaTime;
	if (glfwGetTime() - m_startTime >= 1.0) {
		m_seconds++;
		m_startTime = glfwGetTime();
		MW::App::Log("Second " + std::to_string(m_seconds)
			+ ": " + std::to_string(m_timer) + " updates, "
			+ std::to_string(m_frames) + " frames");
		m_timer = 0.0f;
		m_frames = 0;
	}

	m_sprite.update(deltaTime);

	m_shader.getCamera()->update(deltaTime);
}

void TestScene1::exit() {
	MW::App::Log("Exit scene 1");
}

void TestScene1::destroy() {
	MW::App::Log("Destroy scene 1");
}

int main(int argc, char** argv) {
	MW::App::Init(MW::WindowAttributes(glm::ivec2(800, 600), "MWTest"), 60,
		{ &MWTest::TEST_SCENE_1 }, &MWTest::TEST_SCENE_1);
}