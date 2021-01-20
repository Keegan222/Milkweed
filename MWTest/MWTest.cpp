#include <iostream>
#include <cstdlib>
#include <time.h>

#include "MWTest.h"

TestScene1 MWTest::TEST_SCENE_1;

void TestScene1::init() {
	MW::App::LOG << "Init scene 1\n";

	srand((unsigned int)time(0));
	
	m_shader.init(
		"Assets/Shaders/sprite_vertex_shader.glsl",
		"Assets/Shaders/sprite_fragment_shader.glsl",
		MW::Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix"
	);
	m_textShader.init(
		"Assets/Shaders/text_vertex_shader.glsl",
		"Assets/Shaders/text_fragment_shader.glsl",
		MW::Shader::getDefaultVertexAttributes("inPosition", "inTextureCoords"),
		"cameraMatrix"
	);
	
	m_sprite = MW::AnimatedSprite(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec2(100.0f, 100.0f), MW::App::RESOURCES.getTexture(
			"Assets/Textures/sheet.png"), glm::ivec2(3, 2), 60.0f);
	m_sprite1 = MW::Sprite(glm::vec3(100.0f, 100.0f, 0.0f),
		glm::vec2(100.0f, 100.0f), MW::App::RESOURCES.getTexture(
		"Assets/Textures/texture0.png"));

	m_music = MW::App::RESOURCES.getSound("Assets/Sounds/music.wav");

	m_font = MW::App::RESOURCES.getFont("Assets/Fonts/arial.ttf");
}

void TestScene1::enter() {
	MW::App::LOG << "Enter scene 1\n";

	MW::App::AUDIO.playMusic(m_music);
	MW::App::AUDIO.setGain(0.0f);
}

void TestScene1::draw() {
	m_frames++;

	m_textShader.upload3fVector("textColor", glm::vec3(1.0f, 0.0f, 1.0f));

	MW::App::RENDERER.submit({ &m_sprite, &m_sprite1 }, &m_shader);
	MW::App::RENDERER.submit(MW::Label("Hello world!", glm::vec3(0.0f), 1.0f,
		glm::vec3(1.0f, 0.0f, 0.0f)), m_font, &m_textShader);
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
		MW::App::AUDIO.playSound(MW::App::RESOURCES.getSound(
			"Assets/Sounds/sound.wav"));

		if (MW::App::RENDERER.getClearColor() == glm::vec3()) {
			MW::App::RENDERER.setClearColor(glm::vec3(1.0f, 0.0f, 1.0f));
		}
		else {
			MW::App::RENDERER.setClearColor(glm::vec3(0.0f, 0.0f, 0.0f));
		}
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
		MW::App::LOG << "Second " << m_seconds << ": " << m_timer
			<< " updates, " << m_frames << " frames\n";
		m_UPSCounts.push_back((float)m_timer);
		m_frameCounts.push_back((float)m_frames);
		m_timer = 0.0f;
		m_frames = 0;
	}

	m_sprite.update(deltaTime);

	m_shader.getCamera()->update(deltaTime);
}

void TestScene1::exit() {
	MW::App::LOG << "Exit scene 1\n";
	float aUPS = average(m_UPSCounts);
	float aFPS = average(m_frameCounts);
	MW::App::LOG << "Average UPS: " << aUPS << ", Average FPS: "
		<< aFPS << " over " << m_seconds << " seconds\n";
}

void TestScene1::destroy() {
	MW::App::LOG << "Destroy scene 1\n";
}

float TestScene1::average(const std::vector<float>& v) {
	if (v.size() == 0) {
		return 0.0f;
	}

	float s = 0.0f;
	for (float f : v) {
		s += f;
	}
	return s / v.size();
}

int main(int argc, char** argv) {
	MW::App::Init(MW::WindowAttributes(glm::ivec2(800, 600), "MWTest"), 60,
		{ &MWTest::TEST_SCENE_1 }, &MWTest::TEST_SCENE_1);
}