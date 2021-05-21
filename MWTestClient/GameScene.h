/*
* File:		GameScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

/*
* Enumeration of the types of message for the Test Server/Client system
*/
enum MessageTypes : unsigned int {
	PLAYER_ID_ASSIGNMENT = 2,
	PLAYER_CONNECTED = 3,
	PLAYER_MOVEMENT = 4,
	PLAYER_DISCONNECTED = 5,
};

/*
* The main scene of the Milkweed framework's network testing client.
*/
class GameScene : public Scene {
public:
	/*
	* Initialize the scene.
	*/
	void init() override;
	/*
	* Enter the scene.
	*/
	void enter() override;
	/*
	* Draw the scene's graphics to the screen.
	*/
	void draw() override;
	/*
	* Process input to the scene.
	*/
	void processInput() override;
	/*
	* Process a message recieved from a test server
	*/
	void processNetMessage(NetMessage& message);
	/*
	* Process a UI component event to the scene.
	*/
	void componentEvent(unsigned int groupID, unsigned int componentID,
		unsigned int eventID) override;
	/*
	* Update the size of the scene in the window.
	*/
	void updateWindowSize() override;
	/*
	* Update the physics of the scene.
	*/
	void update(float deltaTime) override;
	/*
	* Exit the scene.
	*/
	void exit() override;
	/*
	* Free the scene's memory.
	*/
	void destroy() override;

private:
	// The address to connect to
	std::string m_address = "127.0.0.1";
	// The port to connect on
	unsigned int m_port = 2773;
	// Whether the client is connect to the server
	bool m_connected = false;
	// Whether the server has authorized this client
	bool m_authorized = false;
	// The camera used to draw sprites
	Camera m_spriteCamera;
	// The shader used to draw sprites
	Shader m_spriteShader;
	// The ID of this player on the server
	unsigned int m_playerID = 0;
	// The other players' sprites with their ID's on the server, including this
	// player
	std::map<unsigned int, Sprite> m_players;
	// Pointers to the other players' sprites
	std::vector<Sprite*> m_playerPointers;
};

#endif