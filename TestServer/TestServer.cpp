#include <iostream>

#include "TestServer.h"

bool TestServer::onConnect(std::shared_ptr<MW::NetConnection> client) {
	unsigned int ID = client->getID();
	m_log << "Connected client with ID " << ID << "\n";

	return true;
}

void TestServer::onMessage(const MW::NetMessage& message) {
	unsigned int ID = message.owner->getID();
	m_log << "Message from " << ID << " with ID " << message.header.ID
		<< " and size " << message.header.size << "\n";
	m_log << "Sending message to all clients except " << ID << "\n";

	messageAllClients(message, message.owner);
}

void TestServer::onDisconnect(std::shared_ptr<MW::NetConnection> client) {
	unsigned int ID = client->getID();
	m_log << "Disconnected client with ID " << ID << "\n";
}

int main() {
	TestServer server(2773);
	server.init(1024);

	while (server.isActive()) {
		server.update(-1);
	}

	server.destroy();

	return 0;
}