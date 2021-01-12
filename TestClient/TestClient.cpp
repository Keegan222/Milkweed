#include "TestClient.h"

using namespace MW;

void TestClient::init() {
	App::LOG << "Initialize test client\n";
	App::NETWORK.connect("127.0.0.1", 2773);
}

void TestClient::enter() {
	App::LOG << "Enter test client\n";
}

void TestClient::draw() {

}

void TestClient::processInput() {
	if (App::INPUT.isButtonPressed(GLFW_MOUSE_BUTTON_1)) {
		glm::vec2 cursor = App::INPUT.getCursorPosition();
		App::LOG << "Mouse pressed at (" << cursor.x << ", " << cursor.y
			<< ")\n";
		NetMessage msg;
		msg.header.ID = 50;
		msg << cursor.x << cursor.y;
		if (App::NETWORK.isConnected()) {
			App::LOG << "Connected, sending message with coords\n";
			App::NETWORK.send(msg);
		}
		else {
			App::LOG << "Disconnected, can't send coords :(\n";
		}
	}
}

void TestClient::update(float deltaTime) {
	while (!App::NETWORK.getMessagesIn().empty()) {
		NetMessage message = App::NETWORK.getMessagesIn().popFront();
		App::LOG << "Message received from server with ID " << message.header.ID
			<< " and size " << message.header.size << "\n";
		if (message.header.ID == 50) {
			float x, y;
			message >> y >> x;
			App::LOG << "Message contains mouse coords (" << x << ", " << y
				<< ")\n";
		}
	}
}

void TestClient::exit() {
	App::LOG << "Exit test client\n";
}

void TestClient::destroy() {
	App::LOG << "Destroy test client\n";
}

int main() {
	TestClient client;
	App::Init(WindowAttributes(glm::ivec2(300, 300), "TestClient"),
		60.0f, { &client }, &client);
	return 0;
}