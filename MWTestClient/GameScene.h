/*
* File:		GameScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <Milkweed/MW.h>

#include "ClientPlayer.h"

using namespace Milkweed;

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
	void gamepadConnected(int gp) override;
	void gamepadDisconnected(int gp) override;
	/*
	* Process a message recieved from a test server
	*/
	void processNetMessage(NetMessage& message) override;
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
	/*
	* Set the username to connect to the server with
	*/
	void setUsername(const std::string& username) { m_username = username; }
	/*
	* Set the address for the game scene to connect to
	*/
	void setAddress(const std::string& address) { m_address = address; }
	/*
	* Set the port for the game scene to connect on
	*/
	void setPort(unsigned int port) { m_port = port; }
	/*
	* Get the player ID of this client on the server
	*/
	unsigned int getPlayerID() const { return m_playerID; }

private:
	const static unsigned int PAUSE_UI_GROUP = 0;
	const static unsigned int HUD_UI_GROUP = 1;
	// The username to connect to the server with
	std::string m_username = "";
	// The address to connect to
	std::string m_address = "127.0.0.1";
	// The port to connect on
	unsigned int m_port = 2773;
	// Whether this client is connected to a server
	bool m_connected = false;
	// Whether this client has received an acceptance message from the server
	bool m_accepted = false;
	// The player ID assigned to this client
	unsigned int m_playerID = 0;
	// Pointers to the sprites of all the connected players
	std::vector<Sprite*> m_playerPointers;
	// The players connected to this server
	std::map<unsigned int, ClientPlayer> m_players;
	// The camera used to draw sprites
	Camera m_spriteCamera;
	// The shader used to draw sprites
	Shader m_spriteShader;
	// The shader used to draw text on sprites
	Shader m_spriteTextShader;
	// The font to use to draw text in this scene
	Font* m_font = nullptr;
	// The camera used to draw the pause UI
	Camera m_UICamera;
	// The shader used to draw UI sprites
	Shader m_UISpriteShader;
	// The shader used to draw text
	Shader m_UITextShader;
	// The UI group for the pause menu
	bool m_pauseMenuUp = false;
	UI::UIGroup m_pauseUIGroup;
	Sprite m_pauseBackground;
	UI::Button m_optionsButton, m_disconnectButton;
	// The UI group for the game HUD
	UI::UIGroup m_HUDUIGroup;
	UI::TextArea m_statsArea;
	// The floor sprite
	Sprite m_floorSprite;

	/*
	* Update the stats text area
	*/
	void updateStatsArea();
	/*
	* Disconnect this client from the network and fall back to the connect scene
	*/
	void disconnect();

	void setComponentDirections();
};

#endif