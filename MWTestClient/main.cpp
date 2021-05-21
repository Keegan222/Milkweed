/*
* File:		main.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Date:		2021.05.19
*/

#include "TitleScene.h"
#include "OptionsScene.h"
#include "GameScene.h"

using namespace Milkweed;

int main(int argc, char** argv) {
	TitleScene titleScene;
	OptionsScene optionsScene;
	GameScene gameScene;

	MW::Init("Test Client", glm::ivec2(800, 600), false, 60.0f,
		{ &titleScene, &optionsScene, &gameScene }, &titleScene);

	return 0;
}