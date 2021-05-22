/*
* File:		TestClient.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.05.21
*/

#ifndef TEST_CLIENT_H
#define TEST_CLIENT_H

#include "TitleScene.h"
#include "OptionsScene.h"
#include "GameScene.h"

class TestClient {
public:
	static TitleScene TITLE_SCENE;
	static OptionsScene OPTIONS_SCENE;
	static GameScene GAME_SCENE;
};

#endif