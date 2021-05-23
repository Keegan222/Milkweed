/*
* File:		TestClient.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.21
*/

#include "TestClient.h"

using namespace Milkweed;

TitleScene TestClient::TITLE_SCENE;
ConnectScene TestClient::CONNECT_SCENE;
OptionsScene TestClient::OPTIONS_SCENE;
GameScene TestClient::GAME_SCENE;

int main(int argc, char** argv) {
	if (!Options::LoadOptions()) {
		MWLOG(Warning, TestClient Main, "Failed to load options file.");
	}

	MW::Init("Test Client", Options::WINDOW_RESOLUTION, Options::FULL_SCREEN,
		60.0f, (float)Options::VOLUME / 100.0f, { &TestClient::TITLE_SCENE,
			&TestClient::CONNECT_SCENE, &TestClient::OPTIONS_SCENE,
			& TestClient::GAME_SCENE }, & TestClient::TITLE_SCENE);

	return 0;
}