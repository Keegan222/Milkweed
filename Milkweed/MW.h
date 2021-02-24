/*
* File: MW.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.1427
*/

#ifndef MW_MW_H
#define MW_MW_H

#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Network.h"
#include "Window.h"
#include "Input.h"
#include "Renderer.h"
#include "Resources.h"
#include "Logging.h"
#include "Audio.h"

namespace MW {
	/*
	* An abstract scene in a Milkweed engine application
	*/
	class Scene {
	public:
		/*
		* Initialize this scene on the start of the application
		*/
		virtual void init() = 0;
		/*
		* Switch to this scene from another scene
		*/
		virtual void enter() = 0;
		/*
		* Draw this scene's graphics on the window
		*/
		virtual void draw() = 0;
		/*
		* Process user input to this scene
		*/
		virtual void processInput() = 0;
		/*
		* Update this scene's physics
		* 
		* @param deltaTime: The elapsed time in frames for this scene based on
		* the frame rate, always between 0.0f and 1.0f
		*/
		virtual void update(float deltaTime) = 0;
		/*
		* Switch from this scene to another one
		*/
		virtual void exit() = 0;
		/*
		* Free this scene's memory
		*/
		virtual void destroy() = 0;
	};

	/*
	* The main class of the Milkweed engine, provides static access to all of
	* the engine's functionality
	*/
	class App {
	public:
		// The application's window
		static Window& WINDOW;
		// The application's input manager
		static InputManager& INPUT;
		// The application's renderer
		static Renderer& RENDERER;
		// The application's resource manager
		static ResourceManager& RESOURCES;
		// The application's logging system
		static LogManager& LOG;
		// The application's network client
		static NetClient& NETWORK;
		// The application audio player
		static AudioManager& AUDIO;

		// Disable the copy constructor
		App(App& a) = delete;
		/*
		* Set up and start an application using the Milkweed engine
		* 
		* @param windowAttrib: The set of window attributes to open the
		* application's window with
		* @param physicsUPS: The target number of physics updates per second
		* @param scenes: The set of scenes the application will/may use
		* @param scene: The initial scene to be active in the application
		*/
		static void Init(const std::string& windowTitle,
			const glm::ivec2& windowDimensions, bool windowFullScreen,
			float physicsUPS, const std::vector<Scene*>& scenes, Scene* scene);
		/*
		* Change the active scene of this application
		* 
		* @param scene: The scene to switch to, must be contained in the list
		* of scenes passed into Init()
		* @return Whether the scene was switched successfully
		*/
		static bool SetScene(Scene* scene);

	private:
		// The number of physics upates per second
		static float PHYSICS_SPU;
		// The set of scenes in this application
		static std::vector<Scene*> SCENES;
		// The active scene in this application
		static Scene* SCENE;

		/*
		* Disable the constructor
		*/
		App() {}
		/*
		* Run the main loop of this application
		*/
		static void Run();
		/*
		* Draw the application's graphics
		*/
		static void Draw();
		/*
		* Process input to the application
		*/
		static void ProcessInput();
		/*
		* Update the application's physics
		* 
		* @param deltaTime: The time elapsed in frames
		*/
		static void Update(float deltaTime);
		/*
		* Free the Milkweed engine application's memory and terminate
		*/
		static void Destroy();
	};
}

#endif