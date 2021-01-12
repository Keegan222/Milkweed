#pragma once

#include <Milkweed/MW.h>

enum class MessageType : unsigned int {
	SERVER_ACCEPT,
	SERVER_REJECT,
};

class TestClient : public MW::Scene {
public:
	void init();
	void enter();
	void draw();
	void processInput();
	void update(float deltaTime);
	void exit();
	void destroy();
};