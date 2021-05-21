/*
* File: MW.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.20
*/

#include <iostream>

#include "MW.h"

using namespace Milkweed;

// Instantiate the application's public static members
Window& MW::WINDOW = Window::getInstance();
InputManager& MW::INPUT = InputManager::getInstance();
Renderer& MW::RENDERER = Renderer::getInstance();
ResourceManager& MW::RESOURCES = ResourceManager::getInstance();
LogManager& MW::LOG = LogManager::getInstance();
NetClient& MW::NETWORK = NetClient::getInstance();
AudioManager& MW::AUDIO = AudioManager::getInstance();
bool MW::RUNNING = false;

// Instantiate the application's private static members
float MW::PHYSICS_SPU;
std::vector<Scene*> MW::SCENES;
Scene* MW::SCENE = nullptr;

void Scene::init() {
	m_initialized = true;
}

void Scene::destroy() {
	m_initialized = false;
}

void MW::Init(const std::string& windowTitle,
	const glm::ivec2& windowDimensions, bool windowFullScreen,
	float physicsUPS, const std::vector<Scene*>& scenes, Scene* scene) {
	// Initialize the logging system
#ifdef _DEBUG
	LOG.init("mwlog/", true);
#else
	LOG.init("mwlog/", false);
#endif

	MWLOG(Info, App, "Initializing Milkweed framework with physics UPS ",
		physicsUPS);

	// Set the physics seconds per update
	PHYSICS_SPU = 1.0f / physicsUPS;

	// Initialize the window
	if (!WINDOW.init(windowTitle, windowDimensions, windowFullScreen)) {
		// Could not open the window
		MWLOG(Error, App, "Fatal window error");
		return;
	}
	// Initialize the input manager
	INPUT.init();
	
	// Initialize the renderer
	if (!RENDERER.init()) {
		MWLOG(Error, App, "Fatal graphical error");
		return;
	}

	// Initialize the resource manager
	RESOURCES.init();

	// Initialize the audio manager
	if (!AUDIO.init()) {
		MWLOG(Error, App, "Fatal audio error");
	}

	// Initialize the networking system
	// TODO: Remove hardcoded message size
	NETWORK.init();

	// Populate the App's list of scenes
	for (Scene* s : scenes) {
		SCENES.push_back(s);
	}
	// Enter the starting scene
	SetScene(scene);

	MWLOG(Info, App, "Initialized Milkweed framework application");
	RUNNING = true;

	// The Milkweed framework has been initialized
	Run();
}

bool MW::SetScene(Scene* scene) {
	// If the scene to switch to is nullptr or not in the list do not switch
	if (scene == nullptr) {
		return false;
	}
	if (std::find(SCENES.begin(), SCENES.end(), scene) == SCENES.end()) {
		return false;
	}

	if (!scene->isInitialized()) {
		scene->init();
	}

	// If the current scene is not nullptr, leave it
	if (SCENE != nullptr) {
		SCENE->exit();
	}

	// Switch to the new scene and enter it
	SCENE = scene;
	SCENE->enter();

	return true;
}

void MW::Run() {
	// Set up physics time-keeping variables
	double startTime = glfwGetTime();
	unsigned int physicsSteps = 0, maxPhysicsSteps = 10;

	// Start the game loop
	while (RUNNING) {
		// Draw the application's graphics and process input
		Draw();
		ProcessInput();

		// Find the elapsed time since last frame
		double now = glfwGetTime();
		float elapsed = (float)(now - startTime);
		startTime = now;
		// Calculate delta time by the quotient of the elapsed time in
		// seconds and the seconds per update
		float deltaTime = elapsed / PHYSICS_SPU;
		// Update while the delta time is greater than one and we haven't
		// exceeded the maximum physics updates per frame (no more than 1.0f
		// per update)
		while (deltaTime > 1.0f && physicsSteps < maxPhysicsSteps) {
			Update(1.0f);
			deltaTime -= 1.0f;
			physicsSteps++;
		}
		// Update with the remaining delta time and reset the steps counter
		Update(deltaTime);
		physicsSteps = 0;

		// Test if the user has requested the window be closed
		if (glfwWindowShouldClose(WINDOW.getWindowHandle())) {
			RUNNING = false;
		}
	}

	// The game loop has stopped, termimate the application
	Destroy();
}

void MW::Draw() {
	RENDERER.begin();
	SCENE->draw();
	RENDERER.end();
	glfwSwapBuffers(WINDOW.getWindowHandle());
}

void MW::ProcessInput() {
	INPUT.update();
	SCENE->processInput();
}

void MW::Update(float deltaTime) {
	SCENE->update(deltaTime);
}

void MW::Destroy() {
	MWLOG(Info, App, "Destroying Milkweed framework application, leaving ",
		"and destroying scenes");
	// Exit the active scene and destroy all the scenes
	SCENE->exit();
	for (Scene* s : SCENES) {
		if (s->isInitialized()) {
			s->destroy();
		}
	}

	// Destroy the resource manager
	RESOURCES.destroy();
	// Stop the audio system
	AUDIO.destroy();
	// Stop the network client
	NETWORK.destroy();
	// Destroy the renderer
	RENDERER.destroy();

	// Destroy the logging system
	LOG.destroy();
}