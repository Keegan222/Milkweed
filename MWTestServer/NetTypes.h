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
	* Contents in unpacking order:
	* - The ID of the newly connected player (unsigned int).
	*/
	CONNECT_PLAYER = 0,
	/*
	* Contents in unpacking order:
	* - The ID of the newly accepted player (unsigned int).
	* - The count of other players on the server (int).
	* - A series of the other players' player IDs (unsigned int),
	* positions (glm::vec3), and velocities (glm::vec2).
	*/
	ACCEPT_PLAYER = 1,
	/*
	* Contents in unpacking order:
	* - The length of the requested username (int).
	* - A series of characters of the requested username (char).
	*/
	USERNAME_REQUEST = 2,
	/*
	* Contents in unpacking order:
	* - The ID of the player being assigned a username (unsigned int).
	* - The length of the assigned username (int).
	* - A series of characters of the assigned username (char).
	*/
	USERNAME_ASSIGNMENT = 3,
	/*
	* Contents in unpacking order:
	* - None, but should be echoed back if the server is in a good mood.
	*/
	PING = 4,
	/*
	* Contents in unpacking order:
	* - The ID of the newly disconnected player.
	*/
	DISCONNECT_PLAYER = 5,
	/*
	* Contents in unpacking order:
	* - The position of the player after requesting a movement left.
	* - The velocity of the player after requesting a movement left.
	*/
	MOVEMENT_LEFT = 6,
	/*
	* Contents in unpacking order:
	* - The position of the player after requesting a movement right.
	* - The velocity of the player after requesting a movement right.
	*/
	MOVEMENT_RIGHT = 7,
	/*
	* Contents in unpacking order:
	* - The position of the player after stopping a movement left.
	* - The velocity of the player after stopping a movement left.
	*/
	MOVEMENT_STOP_LEFT = 8,
	/*
	* Contents in unpacking order:
	* - The position of the player after stopping a movement right.
	* - The velocity of the player after stopping a movement right.
	*/
	MOVEMENT_STOP_RIGHT = 9,
	/*
	* Contents in unpacking order:
	* - The position of the player after requesting a jump.
	* - The velocity of the player after requesting a jump.
	*/
	MOVEMENT_JUMP = 10,
	/*
	* Contents in unpacking order:
	* - The ID of the player being updated.
	* - The position of the player.
	* - The velocity of the player.
	*/
	PLAYER_PV_UPDATE = 11,
};

// Shared definition for game qualities
#define PLAYER_SPAWNPOINT glm::vec3(0.0f, 0.0f, 0.0f)
#define PLAYER_DIMENSIONS glm::vec2(35.0f, 60.0f)
#define GRAVITY 0.981f
#define PLAYER_SPEED_X 5.0f
#define PLAYER_JUMP_SPEED 17.0f
#define MIN_VELOCITY_Y -35.0f
#define TOWN_FLOOR_Y 0.0f
#define TOWN_BORDER_LEFT 0.0f
#define TOWN_BORDER_RIGHT 1500.0f

/*
* Structure for all information representing a general player
*/
class Player : public Sprite {
public:
	// The username of this player
	std::string username = "";
	// Whether this player is in a jump
	bool jumping = false;

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
		if (this->position.x < TOWN_BORDER_LEFT) {
			this->position.x = TOWN_BORDER_LEFT;
		}
		else if (this->position.x > TOWN_BORDER_RIGHT - PLAYER_DIMENSIONS.x) {
			this->position.x = TOWN_BORDER_RIGHT - PLAYER_DIMENSIONS.x;
		}
		if (this->position.y < TOWN_FLOOR_Y) {
			this->position.y = TOWN_FLOOR_Y;
			this->velocity.y = 0.0f;
			this->jumping = false;
		}
	};
};

#endif