/*
* File: MW.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.2018
*/

#include <iostream>

#include "MW.h"

namespace MW {
	// Instantiate the application's public static members
	Window App::WINDOW;
	InputManager App::INPUT;
	Renderer App::RENDERER;
	ResourceManager App::RESOURCES;
	AudioManager App::AUDIO;
	LogManager App::LOG;
	NetClient App::NETWORK;

	// Instantiate the application's private static members
	float App::PHYSICS_SPU;
	std::vector<Scene*> App::SCENES;
	Scene* App::SCENE = nullptr;

	void App::Init(const WindowAttributes& windowAttrib, float physicsUPS,
		const std::vector<Scene*>& scenes, Scene* scene) {
		// Initialize the logging system
#ifdef _DEBUG
		LOG.init(true);
#else
		LOG.init(false);
#endif

		// Set the physics seconds per update
		PHYSICS_SPU = 1.0f / physicsUPS;

		// Initialize the window
		if (!WINDOW.init(windowAttrib)) {
			// Could not open the window
			LOG << "Failed to open the application's window\n";
			return;
		}

		// Initialize the input manager
		INPUT.init();
		
		// Initialize the renderer
		RENDERER.init();

		// Initialize audio system
		if (!AUDIO.init()) {
			LOG << "Failed to initialize the audio system\n";
			return;
		}

		// Initialize the resource manager
		if (!RESOURCES.init()) {
			LOG << "Failed to initialize FreeType, "
				<< "font loading will be disabled\n";
		}

		// Initialize the networking system
		// TODO: Remove hardcoded message size
		NETWORK.init(1024);

		// Initialize the application's scenes and set the initial scene
		for (Scene* s : scenes) {
			SCENES.push_back(s);
			s->init();
		}
		SetScene(scene);

		// The Milkweed engine has been initialized
		LOG << "Milkweed engine initialized\n";
		Run();
	}

	bool App::SetScene(Scene* scene) {
		// If the scene to switch to is nullptr or not in the list do not switch
		if (scene == nullptr) {
			return false;
		}
		if (std::find(SCENES.begin(), SCENES.end(), scene) == SCENES.end()) {
			return false;
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

	void App::Run() {
		// Set up physics time-keeping variables
		double startTime = glfwGetTime();
		unsigned int physicsSteps = 0, maxPhysicsSteps = 10;

		// Start the game loop
		while (!glfwWindowShouldClose(WINDOW.getWindowHandle())) {
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
		}

		// The game loop has stopped, termimate the application
		Destroy();
	}

	void App::Draw() {
		RENDERER.begin();
		SCENE->draw();
		RENDERER.end();
		glfwSwapBuffers(WINDOW.getWindowHandle());
	}

	void App::ProcessInput() {
		INPUT.update();
		SCENE->processInput();
	}

	void App::Update(float deltaTime) {
		SCENE->update(deltaTime);
	}

	void App::Destroy() {
		LOG << "Milkweed engine application stopped, cleaning up\n";
		
		// Exit the active scene and destroy all the scenes
		SCENE->exit();
		for (Scene* s : SCENES) {
			s->destroy();
		}
		
		// Stop the network client
		NETWORK.destroy();
		// Stop music and sound effects so they can be destroyed in the resource
		// manager
		AUDIO.stop();
		// Destroy the resource manager
		RESOURCES.destroy();
		// Destroy the renderer
		RENDERER.destroy();
		// Destroy the audio system
		AUDIO.destroy();

		LOG << "Milkweed engine stopped\n";

		// Destroy the logging system
		LOG.destroy();
	}
}