/*
* File:		TestServer.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <Milkweed/MW.h>

#include "ServerPlayer.h"

using namespace Milkweed;

/*
* The main class of the Milkweed framework's network testing server.
*/
class TestServer : public NetServer {
public:
	/*
	* Construct the TestServer with a port.
	*/
	TestServer(unsigned int port) : NetServer(port) {}
	/*
	* Destroy this server on deletion.
	*/
	~TestServer();
	/*
	* Update the physics of the server
	* 
	* @param deltaTime: The elapsed time since the last frame
	*/
	void updatePhysics(float deltaTime);

protected:
	/*
	* A client has connected to the TestServer.
	*/
	bool onConnect(std::shared_ptr<NetConnection> client) override;
	/*
	* A NetMessage has been sent by a client.
	*/
	void onMessage(NetMessage& message) override;
	/*
	* A client has disconnected from the TestServer.
	*/
	void onDisconnect(std::shared_ptr<NetConnection> client) override;

private:
	// Map from client ID numbers to player information structures
	std::map<unsigned int, ServerPlayer> m_players;

	void publishPlayerUsername(unsigned int clientID, int destID = -1);
	void sendPlayerPVUpdate(NetMessage& message);
	bool validatePlayerMovement(unsigned int clientID,
		const glm::vec3& position, const glm::vec2& velocity);
};

#endif