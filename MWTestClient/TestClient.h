/*
* File:		TestClient.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.21
*/

#ifndef TEST_CLIENT_H
#define TEST_CLIENT_H

#include "IntroScene.h"
#include "TitleScene.h"
#include "ConnectScene.h"
#include "OptionsScene.h"
#include "GameScene.h"

class TestClient {
public:
	static IntroScene INTRO_SCENE;
	static TitleScene TITLE_SCENE;
	static ConnectScene CONNECT_SCENE;
	static OptionsScene OPTIONS_SCENE;
	static GameScene GAME_SCENE;
};

#endif