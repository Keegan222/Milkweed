/*
* File:		ClientPlayer.cpp
* Author:	Keegan MacDonald (keeganm742@gmail.com)
* Created:	2021.06.15
*/

#include "GameScene.h"
#include "ClientPlayer.h"

#define X_AXIS_THRESHOLD 0.85f

void ClientPlayer::init(GameScene* parent, unsigned int clientID) {
	this->parent = parent;
	this->clientID = clientID;
	this->position = PLAYER_SPAWNPOINT;
	this->velocity = glm::vec2(0.0f, 0.0f);
	this->dimensions = PLAYER_DIMENSIONS;
	if (clientID == parent->getPlayerID()) {
		this->texture = MW::RESOURCES.getTexture(
			"Assets/texture/self.png");
	}
	else {
		this->texture = MW::RESOURCES.getTexture(
			"Assets/texture/other.png");
	}
	this->textureCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	this->rotation = 0;
	this->flipHorizontal = false;
	this->flipVertical = false;
}

void ClientPlayer::processInput() {
	if (MW::INPUT.getGamepadCount() > 0) {
		// Take input from gamepad
		int gp = 0;
		if (MW::INPUT.isGamepadAxisMoved(GA_LEFT_X, nullptr, &gp)) {
			float apos = MW::INPUT.getGamepadAxisPosition(gp, GA_LEFT_X);
			if (apos > X_AXIS_THRESHOLD) {
				if (!m_gpRight) {
					m_gpRight = true;
					moveRight(true);
				}
			}
			else if (m_gpRight) {
				m_gpRight = false;
				moveRight(false);
			}
			if (apos < -X_AXIS_THRESHOLD) {
				if (!m_gpLeft) {
					m_gpLeft = true;
					moveLeft(true);
				}
			}
			else if (m_gpLeft) {
				m_gpLeft = false;
				moveLeft(false);
			}
		}
		if (MW::INPUT.isGamepadButtonPressed(G_A, &gp)) {
			jump();
		}
	}
	else {
		// Take input from keyboard
		if (MW::INPUT.isKeyPressed(K_A) && this->velocity.x >= 0.0f) {
			moveLeft(true);
		}
		if (MW::INPUT.isKeyReleased(K_A) && this->velocity.x != 0.0f) {
			moveLeft(false);
		}
		if (MW::INPUT.isKeyPressed(K_D) && this->velocity.x <= 0.0f) {
			moveRight(true);
		}
		if (MW::INPUT.isKeyReleased(K_D) && this->velocity.x != 0.0f) {
			moveRight(false);
		}
		if (MW::INPUT.isKeyPressed(S_SPACE)) {
			jump();
		}
	}
}

void ClientPlayer::destroy() {
	this->parent = nullptr;
	this->clientID = 0;
	this->position = glm::vec3();
	this->velocity = glm::vec2();
	this->dimensions = glm::vec2();
	this->texture = nullptr;
	this->textureCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	this->rotation = 0;
	this->flipHorizontal = false;
	this->flipVertical = false;
}

void ClientPlayer::moveLeft(bool start) {
	if (start) {
		this->velocity.x = -PLAYER_SPEED_X;
		NetMessage moveMsg;
		moveMsg.header.ID = MOVEMENT_LEFT;
		moveMsg << this->velocity << this->position;
		MW::NETWORK.send(moveMsg);
	}
	else {
		if (this->velocity.x < 0.0f) {
			this->velocity.x = 0.0f;
		}
		NetMessage moveMsg;
		moveMsg.header.ID = MOVEMENT_STOP_LEFT;
		moveMsg << this->velocity << this->position;
		MW::NETWORK.send(moveMsg);
	}
}

void ClientPlayer::moveRight(bool start) {
	if (start) {
		this->velocity.x = PLAYER_SPEED_X;
		NetMessage moveMsg;
		moveMsg.header.ID = MOVEMENT_RIGHT;
		moveMsg << this->velocity << this->position;
		MW::NETWORK.send(moveMsg);
	}
	else {
		if (this->velocity.x > 0.0f) {
			this->velocity.x = 0.0f;
		}
		NetMessage moveMsg;
		moveMsg.header.ID = MOVEMENT_STOP_RIGHT;
		moveMsg << this->velocity << this->position;
		MW::NETWORK.send(moveMsg);
	}
}

void ClientPlayer::jump() {
	// Can't initiate a jump if already in one
	if (this->jumping) {
		return;
	}
	this->jumping = true;
	this->velocity.y = PLAYER_JUMP_SPEED;
	// Send the jump input to the server
	NetMessage jmsg;
	jmsg.header.ID = MessageTypes::MOVEMENT_JUMP;
	jmsg << this->velocity << this->position;
	MW::NETWORK.send(jmsg);
}