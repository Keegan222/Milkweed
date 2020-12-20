#include <iostream>
#include <cstdlib>
#include <time.h>
#include <ft2build.h>
#include <freetype/freetype.h>

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

	m_music = MW::App::RESOURCES.getSound("Assets/Sounds/music.wav");
}

void TestScene1::enter() {
	MW::App::Log("Enter scene 1");

	MW::App::AUDIO.playMusic(m_music);
}

void TestScene1::draw() {
	m_frames++;

	MW::App::RENDERER.submit({ &m_sprite, &m_sprite1 }, &m_shader);
}

void TestScene1::processInput() {
	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_SPACE)) {
		if (MW::App::AUDIO.isMusicPlaying()) {
			MW::App::AUDIO.pauseMusic();
		}
		else {
			MW::App::AUDIO.playMusic();
		}
	}

	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_S)) {
		if (MW::App::AUDIO.isMusicPlaying()) {
			MW::App::AUDIO.stopMusic();
		}
		else {
			if (MW::App::AUDIO.isMusicPaused()) {
				MW::App::AUDIO.stopMusic();
			}
			else {
				MW::App::AUDIO.playMusic(m_music);
			}
		}
	}

	if (MW::App::INPUT.isButtonPressed(GLFW_MOUSE_BUTTON_1)) {
		MW::App::Log("Pressed");
		MW::App::AUDIO.playSound(MW::App::RESOURCES.getSound(
			"Assets/Sounds/sound.wav"));
	}

	if (MW::App::INPUT.isKeyPressed(GLFW_KEY_UP)) {
		MW::App::AUDIO.setGain(MW::App::AUDIO.getGain() + 0.1f);
	}
	else if (MW::App::INPUT.isKeyPressed(GLFW_KEY_DOWN)) {
		MW::App::AUDIO.setGain(MW::App::AUDIO.getGain() - 0.1f);
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