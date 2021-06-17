/*
* File:		IntroScene.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.16
*/

#include "TestClient.h"
#include "IntroScene.h"

void IntroScene::init() {
	MWLOG(Info, IntroScene, "Initialized");
}

void IntroScene::enter() {
	MWLOG(Info, IntroScene, "Entered");
	incrementSlide();
}

void IntroScene::draw() {
	
}

void IntroScene::processInput() {
	if (MW::INPUT.isButtonPressed(B_LEFT)) {
		if (++m_slide >= SLIDE_COUNT) {
			MW::SetScene(&TestClient::TITLE_SCENE);
			return;
		}
		incrementSlide();
	}
}

void IntroScene::componentEvent(unsigned int groupID, unsigned int componentID,
	unsigned int eventID) {

}

void IntroScene::updateWindowSize() {

}

void IntroScene::update(float deltaTime) {

}

void IntroScene::exit() {
	MWLOG(Info, IntroScene, "Exited");
	MW::RENDERER.setClearColor(glm::vec3(0.0f, 0.0f, 0.0f));
}

void IntroScene::destroy() {
	MWLOG(Info, IntroScene, "Destroyed");
}

void IntroScene::incrementSlide() {
	MWLOG(Info, IntroScene, "Moved to slide ", m_slide);
	MW::RENDERER.setClearColor(glm::vec3(
		(float)m_slide / (float)SLIDE_COUNT, 0.0f, 0.0f));
}