/*
* File:		TestClient.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.16
*/

#include "TestClient.h"

using namespace Milkweed;

void TestClient::init() {
	MWLOG(Info, TestClient, "Initialized scene");
}

void TestClient::enter() {
	MWLOG(Info, TestClient, "entered scene");
	std::string address = "127.0.0.1";
	unsigned int port = 2773;
	double timeout = 10.0;

	MW::NETWORK.connect(address, port);
	double startTime = glfwGetTime();
	while (glfwGetTime() - startTime < timeout && !m_connected) {
		m_connected = MW::NETWORK.isConnected();
	}

	if (m_connected) {
		MWLOG(Info, TestClient, "Connected");
	}
	else {
		MWLOG(Error, TestClient, "Failed to connect");
	}
}

void TestClient::draw() {

}

void TestClient::processInput() {
	m_connected = MW::NETWORK.isConnected();
	if (!m_connected) {
		return;
	}

	// Process network input here
}

void TestClient::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {

}

void TestClient::updateWindowSize() {

}

void TestClient::update(float deltaTime) {

}

void TestClient::exit() {
	MWLOG(Info, TestClient, "Exited scene");
}

void TestClient::destroy() {
	MWLOG(Info, TestClient, "Destroyed scene");
}

int main(int argc, char** argv) {
	TestClient testClient;
	MW::Init("Test Client", glm::ivec2(800, 600), false, 60.0f, { &testClient },
		&testClient);
	testClient.destroy();
	return 0;
}