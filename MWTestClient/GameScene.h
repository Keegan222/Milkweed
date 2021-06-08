/*
* File:		GameScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <MWTestServer/NetTypes.h>
#include <Milkweed/MW.h>

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
	/*
	* Set the address for the game scene to connect to
	*/
	void setAddress(const std::string& address) { m_address = address; }
	/*
	* Set the port for the game scene to connect on
	*/
	void setPort(unsigned int port) { m_port = port; }

private:
	const static unsigned int PAUSE_UI_GROUP = 0;
	const static unsigned int HUD_UI_GROUP = 1;
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
	// The players connected to this server
	std::map<unsigned int, Player> m_players;
	// The camera used to draw sprites
	Camera m_spriteCamera;
	// The shader used to draw sprites
	Shader m_spriteShader;
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

	/*
	* Update the stats text area
	*/
	void updateStatsArea();
	/*
	* Disconnect this client from the network and fall back to the connect scene
	*/
	void disconnect();
};

#endif