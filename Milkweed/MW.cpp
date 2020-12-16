/*
* File: MW.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.2018
*/

#include <time.h>
#include <iostream>
#include <direct.h>

#include "MW.h"

namespace MW {
	// Instantiate the application's public static members
	Window App::WINDOW;
	InputManager App::INPUT;
	Renderer App::RENDERER;
	ResourceManager App::RESOURCES;
	AudioManager App::AUDIO;

	// Instantiate the application's private static members
	std::ofstream App::LOG_STREAM;
	float App::PHYSICS_SPU;
	std::vector<Scene*> App::SCENES;
	Scene* App::SCENE = nullptr;

	std::string App::GetDate() {
		// Get the current time
		time_t now = time(0);
		tm tstruct;
		localtime_s(&tstruct, &now);
		// Write the time into a buffer in the correct format
		char buffer[64];
		strftime(buffer, sizeof(buffer), "%Y.%m.%d.%H%M.%S", &tstruct);
		return std::string(buffer);
	}

	void App::Log(const std::string& message) {
		// If debugging write the message to the console
#ifdef _DEBUG
		std::cout << GetDate() << ": " << message << std::endl;
#endif
		if (LOG_STREAM.fail()) {
			return;
		}
		// If the log stream hasn't failed write the message to the log file
		LOG_STREAM << GetDate() << ": " << message << std::endl;
	}

	void App::Init(const WindowAttributes& windowAttrib, float physicsUPS,
		const std::vector<Scene*>& scenes, Scene* scene) {
		// Initialize the logging system
		_mkdir("mwlog/");
		LOG_STREAM.open("mwlog/" + GetDate() + ".mwlog");
		Log("Starting Milkweed engine");

		// Set the physics seconds per update
		PHYSICS_SPU = 1.0f / physicsUPS;

		// Initialize the window
		if (!WINDOW.init(windowAttrib)) {
			// Could not open the window
			Log("Failed to open the application's window");
			return;
		}

		// Initialize the input manager
		INPUT.init();
		
		// Initialize the renderer
		// TODO: The clear color should not be hardcoded
		RENDERER.init(glm::vec3(0.0f, 0.0f, 0.0f));

		// Initialize audio system
		if (!AUDIO.init()) {
			Log("Failed to initialize the audio system");
			return;
		}

		// Initialize the application's scenes and set the initial scene
		for (Scene* s : scenes) {
			SCENES.push_back(s);
			s->init();
		}
		SetScene(scene);

		// The Milkweed engine has been initialized
		Log("Milkweed engine initialized");
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
		Log("Milkweed engine application stopped, cleaning up");
		
		// Exit the active scene and destroy all the scenes
		SCENE->exit();
		for (Scene* s : SCENES) {
			s->destroy();
		}

		// Destroy the resource manager
		RESOURCES.destroy();
		// Destroy the renderer
		RENDERER.destroy();
		// Destroy the audio system
		AUDIO.destroy();

		Log("Milkweed engine stopped");

		// Stop the logging system
		LOG_STREAM.close();
	}
}