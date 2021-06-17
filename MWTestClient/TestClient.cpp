/*
* File:		TestClient.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.21
*/

#include "TestClient.h"

using namespace Milkweed;

IntroScene TestClient::INTRO_SCENE;
TitleScene TestClient::TITLE_SCENE;
ConnectScene TestClient::CONNECT_SCENE;
OptionsScene TestClient::OPTIONS_SCENE;
GameScene TestClient::GAME_SCENE;

#define FORCE_INTRO false

int main(int argc, char** argv) {
	if (!Options::LoadOptions()) {
		MWLOG(Warning, TestClient Main, "Failed to load options file.");
	}

	Scene* initialScene = nullptr;
	if (Options::INITIALIZED && !FORCE_INTRO) {
		initialScene = &TestClient::TITLE_SCENE;
	}
	else {
		initialScene = &TestClient::INTRO_SCENE;
	}
	MW::Init("Test Client", Options::WINDOW_RESOLUTION, Options::FULL_SCREEN,
		60.0f, (float)Options::VOLUME / 100.0f, { &TestClient::INTRO_SCENE,
			&TestClient::TITLE_SCENE, &TestClient::CONNECT_SCENE,
			&TestClient::OPTIONS_SCENE, & TestClient::GAME_SCENE },
		initialScene);

	Options::SaveOptions();

	return 0;
}