/*
* File:		NetTypes.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.02
* Notes:	This file is included by both the TestServer and TestClient projects
*			and defines common types and simple functions used for networking
*			shared between the two projects.
*/

#ifndef NET_TYPES_H
#define NET_TYPES_H

#include <Milkweed/Sprite.h>

using namespace Milkweed;

/*
* Enumerates the types of messages to be send between clients and server
*/
enum MessageTypes : unsigned int {
	/*
	* Contains the player ID of a newly connected player
	*/
	CONNECT_PLAYER = 0,
	/*
	* Contains the player ID of the newly accepted player and a list of the
	* player ID's of the currently connected players
	*/
	ACCEPT_PLAYER = 1,
	/*
	* No body. Echoed by the server to all clients ignoring the sender
	*/
	PING = 2,
	/*
	* Contains the player ID of the now disconnected player
	*/
	DISCONNECT_PLAYER = 3,
};

#define PLAYER_DIMENSIONS glm::vec2(35.0f, 60.0f)

/*
* Structure for all the information needed to simulate a single player on the
* server side of the network.
*/
class ServerPlayer : public Sprite {
public:
	// The client ID of this player on the server
	unsigned int clientID = 0;
	// The physics timer of this sprite
	float timer = 0.0f;

	/*
	* Construct a blank server player as a sprite
	*/
	ServerPlayer() {};
	/*
	* Construct a server player as a sprite
	* 
	* @param clientID: The client ID number of this player on the server.
	* @param position: The position of this player in the game world.
	* @param velocity: The velocity of this player in the game world.
	*/
	ServerPlayer(unsigned int clientID, const glm::vec3& position,
		const glm::vec2& velocity) {
		this->clientID = clientID;
		this->position = position;
		this->velocity = velocity;
		this->dimensions = PLAYER_DIMENSIONS;
	};
	/*
	* Update the physics of this server player's sprite in the game world.
	* 
	* @param deltaTime: The elapsed time in frames since the last update.
	*/
	void update(float deltaTime) override {
		timer += deltaTime;
		if (timer > 60.0f) {
			timer = 0.0f;
			std::cout << "Updated player physics for one second" << std::endl;
		}
	};
	/*
	* Free this server player's memory
	*/
	void destroy() override {
		this->clientID = 0;
		this->timer = 0.0f;
		this->position = glm::vec3();
		this->velocity = glm::vec2();
		this->dimensions = glm::vec2();
	};
};

/*
* Structure for all the information needed to simulate a single player on the
* client side of the network.
*/
class ClientPlayer : public Sprite {
public:
	// The client ID of this player assigned by the server
	unsigned int clientID = 0;

	/*
	* Construct this client player as a blank sprite.
	*/
	ClientPlayer() {};
	/*
	* Construct this client player with a client ID and sprite information.
	* 
	* @param clientID: The client ID of this player assigned by the server.
	*/
	ClientPlayer(unsigned int clientID) {
		this->clientID = clientID;
	};
};

#endif