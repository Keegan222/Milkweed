/*
* File:		TestServer.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <Milkweed/MW.h>

using namespace Milkweed;

/*
* Enumeration of the types of message for the Test Server/Client system
*/
enum MessageTypes : unsigned int {
	ID_ASSIGNMENT = 0,
	CONNECTION = 1,
	MOVEMENT = 2,
	DISCONNECTION = 3,
};

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
};

#endif