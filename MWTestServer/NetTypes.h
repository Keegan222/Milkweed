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

#include <iostream>
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
	/*
	* A player has inputted a left-movement
	*/
	MOVEMENT_LEFT = 4,
	/*
	* A player has inputted a right-movement
	*/
	MOVEMENT_RIGHT = 5,
	/*
	* A player has released the left-movement input
	*/
	MOVEMENT_STOP_LEFT = 6,
	/*
	* A player has released the right-movement input
	*/
	MOVEMENT_STOP_RIGHT = 7,
	/*
	* A position and velocity update from the server for a connected player
	*/
	PLAYER_PV_UPDATE = 8,
};

// Shared definition for game qualities
#define PLAYER_SPAWNPOINT glm::vec3(0.0f, 0.0f, 0.0f)
#define PLAYER_DIMENSIONS glm::vec2(35.0f, 60.0f)
#define GRAVITY 5.81f
#define PLAYER_SPEED_X 5.0f
#define MIN_VELOCITY_Y -35.0f
#define TOWN_FLOOR_Y 0.0f
#define TOWN_WALL 0.0f

/*
* Structure for all information representing a general player
*/
class Player : public Sprite {
public:
	/*
	* Update the physics of this player
	*/
	void update(float deltaTime) override {
		// Apply gravity to the y-velocity
		this->velocity.y -= GRAVITY * deltaTime;
		if (this->velocity.y < MIN_VELOCITY_Y) {
			this->velocity.y = MIN_VELOCITY_Y;
		}
		this->position.x += this->velocity.x * deltaTime;
		this->position.y += this->velocity.y * deltaTime;
		// Make moving out of bounds impossible
		if (this->position.x < TOWN_WALL) {
			this->position.x = TOWN_WALL;
		}
		if (this->position.y < TOWN_FLOOR_Y) {
			this->position.y = TOWN_FLOOR_Y;
			this->velocity.y = 0.0f;
		}
	};
};

#endif