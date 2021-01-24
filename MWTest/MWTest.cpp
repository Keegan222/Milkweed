#include "MWTest.h"

void TestScene::init() {

}

void TestScene::enter() {

}

void TestScene::draw() {

}

void TestScene::processInput() {

}

void TestScene::update(float deltaTime) {

}

void TestScene::exit() {

}

void TestScene::destroy() {

}

TestScene MWTest::TEST_SCENE;

int main(int argc, char** argv) {
	MW::App::Init(MW::WindowAttributes(glm::ivec2(800, 600), "MWTest"), 60.0f,
		{ &MWTest::TEST_SCENE }, &MWTest::TEST_SCENE);
	return 0;
}