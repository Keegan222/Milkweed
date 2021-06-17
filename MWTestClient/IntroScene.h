/*
* File:		IntroScene.h
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.16
*/

#ifndef INTRO_SCENE_H
#define INTRO_SCENE_H

#include <Milkweed/MW.h>

using namespace Milkweed;

class IntroScene : public Scene {
public:
	const static unsigned int SLIDE_COUNT = 5;

	void init() override;
	void enter() override;
	void draw() override;
	void processInput() override;
	void componentEvent(unsigned int groupID, unsigned int componentID,
		unsigned int eventID) override;
	void updateWindowSize() override;
	void update(float deltaTime) override;
	void exit() override;
	void destroy() override;

protected:
	unsigned int m_slide = 0;

	void incrementSlide();
};

#endif