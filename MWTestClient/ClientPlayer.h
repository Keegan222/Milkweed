/*
* File:		ClientPlayer.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.15
*/

#ifndef CLIENT_PLAYER_H
#define CLIENT_PLAYER_H

#include <MWTestServer/NetTypes.h>

class GameScene;

/*
* Structure for all the information needed to simulate a single player on the
* client side of the network.
*/
class ClientPlayer : public Player {
public:
	// Pointer to the client that owns this player
	GameScene* parent = nullptr;
	// The client ID of this player assigned by the server
	unsigned int clientID = 0;

	/*
	* Construct this client player with a client ID and sprite information.
	*
	* @param parent: A pointer to the client that owns this player.
	* @param clientID: The client ID of this player assigned by the server.
	*/
	void init(GameScene* parent, unsigned int clientID);
	/*
	* Process input to this player
	*/
	void processInput() override;
	/*
	* Free this player's memory
	*/
	void destroy() override;

private:
	bool m_gpLeft = false, m_gpRight = false;

	/*
	* Initiate a move left
	*/
	void moveLeft(bool start);
	/*
	* Initiate a move right
	*/
	void moveRight(bool start);
};

#endif