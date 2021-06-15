/*
* File: MW.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.20
*/

#ifndef MW_MW_H
#define MW_MW_H

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
#include "UI.h"

#define MWLOG(LEVEL, SOURCE, ...) MW::LOG , MW::LOG.getDate(), ": [",\
	#LEVEL, "] [", #SOURCE, "] ", __VA_ARGS__, "\n"

namespace Milkweed {
	/*
	* An abstract scene in a Milkweed framework application
	*/
	class Scene : public InputListener {
	public:
		/*
		* Initialize this scene on the start of the application
		*/
		virtual void init();
		/*
		* Switch to this scene from another scene
		*/
		virtual void enter();
		/*
		* Draw this scene's graphics on the window
		*/
		virtual void draw() = 0;
		/*
		* Process user input to this scene
		*/
		virtual void processInput() = 0;
		/*
		* Process a UI component event on this scene
		* 
		* @param groupID: The ID of the UI group this event originated from
		* @param componentID: The ID of the component this event originated from
		* @param eventID: The ID of the event that has occurred in this component
		*/
		virtual void componentEvent(unsigned int groupID,
			unsigned int componentID, unsigned int eventID) {};
		/*
		* Update the graphics in this scene when the size of the application's
		* window changes
		*/
		virtual void updateWindowSize() {}
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
		virtual void exit();
		/*
		* Free this scene's memory
		*/
		virtual void destroy();
		/*
		* Test whether this scene has been initialized
		*/
		bool isInitialized() const { return m_initialized; }

	protected:
		bool m_initialized = false;
	};

	/*
	* The main class of the Milkweed framework, provides static access to all of
	* the framework's functionality and contains the framework's entry point
	*/
	class MW {
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
		// Whether the application is still running
		static bool RUNNING;

		/*
		* The copy constructor is disabled for this class
		*/
		MW(MW& a) = delete;
		/*
		* Set up and start an application using the Milkweed framework
		*
		* @param windowTitle: The title to appear on this application's window
		* @param windowDimensions: The desired dimensions of this application's
		* window in pixels
		* @param windowFullScreen: Whether this window should initially appear
		* in fullscreen mode
		* @param physicsUPS: The desired number of physics updates to perform
		* per second
		* @param audioGain: The gain of the audio for this application (0 - 1)
		* @param scenes: A vector of pointers to all scenes this application
		* will use while running
		* @param scene: A pointer to the initial scene to be active in the
		* application
		*/
		static void Init(const std::string& windowTitle,
			const glm::ivec2& windowDimensions, bool windowFullScreen,
			float physicsUPS, float audioGain,
			const std::vector<Scene*>& scenes, Scene* scene);
		/*
		* Change the active scene of this application
		*
		* @param scene: The scene to switch to, must be contained in the list
		* of scenes passed into Init()
		* @return Whether the scene was switched successfully
		*/
		static bool SetScene(Scene* scene);

	private:
		// Allow the Window class to access the array of SCENES
		friend Window;

		// The number of physics upates per second
		static float PHYSICS_SPU;
		// The set of scenes in this application
		static std::vector<Scene*> SCENES;
		// The active scene in this application
		static Scene* SCENE;

		/*
		* The constructor is disabled for this class
		*/
		MW() {}
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
		*/
		static void Update(float deltaTime);
		/*
		* Free the Milkweed application's memory and terminate
		*/
		static void Destroy();
	};
}

#endif