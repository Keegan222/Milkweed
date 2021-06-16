/*
* File:		ServerPlayer.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.15
*/

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "NetTypes.h"

class TestServer;

/*
* Structure for all the information needed to simulate a single player on the
* server side of the network.
*/
class ServerPlayer : public Player {
public:
	// Pointer to the server that owns this player
	TestServer* parent = nullptr;
	// The client ID of this player on the server
	unsigned int clientID = 0;

	/*
	* Construct a server player as a sprite
	*
	* @param parent: A pointer to the server that owns this player.
	* @param clientID: The client ID number of this player on the server.
	* @param position: The position of this player in the game world.
	* @param velocity: The velocity of this player in the game world.
	*/
	void init(TestServer* parent, unsigned int clientID,
		const glm::vec3& position, const glm::vec2& velocity);
	/*
	* Free this server player's memory
	*/
	void destroy() override;
};

#endif