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

namespace Milkweed {
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

	/*
	* Structure for all the information needed to simulate a single player
	*/
	struct Player {

	};
}

#endif