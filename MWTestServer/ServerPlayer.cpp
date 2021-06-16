/*
* File:		ServerPlayer.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.15
*/

#include "ServerPlayer.h"

void ServerPlayer::init(TestServer* parent, unsigned int clientID,
	const glm::vec3& position, const glm::vec2& velocity) {
	this->parent = parent;
	this->clientID = clientID;
	this->position = position;
	this->velocity = velocity;
	this->dimensions = PLAYER_DIMENSIONS;
}

void ServerPlayer::destroy() {
	this->parent = nullptr;
	this->clientID = 0;
	this->position = glm::vec3();
	this->velocity = glm::vec2();
	this->dimensions = glm::vec2();
}