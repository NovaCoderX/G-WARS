// PolyNova3D (version 3.3)
/***************************************************************
 Copyright (C) 1999 Novasoft Consulting

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free
 Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************/
#ifndef __PLAYER_H
#define __PLAYER_H

enum PowerUpType {
	NO_POWER_UP_ASSIGNED = 0,
	INCREASED_SPEED_POWER_UP = 1,
	INCREASED_FIRE_RATE_POWER_UP = 2,
	MULTISHOT_POWER_UP = 3,
	HOMING_MISSILE_POWER_UP = 4
};

static std::string DEFAULT_PLAYER_NAME = "DOOMGUY";
#define MAX_PLAYER_NAME_LENGTH 12

struct InputRequest {
	// This represents wants the player wants to do.
	bool left, right, up, down;
	bool fireMissile;
	bool dropBomb;

	InputRequest() {
		left = right = up = down = false;
		fireMissile = dropBomb = false;
	}

	void reset() {
		left = right = up = down = false;
		fireMissile = dropBomb = false;
	}
};

class Player: public Sprite, public ExplosiveObject {
public:
	Player(PlayState* playState);
	~Player();

	void moveDownRequest(bool toggle) {
		inputRequest.down = toggle;
	}

	void moveUpRequest(bool toggle) {
		inputRequest.up = toggle;
	}

	void moveLeftRequest(bool toggle) {
		inputRequest.left = toggle;
	}

	void moveRightRequest(bool toggle) {
		inputRequest.right = toggle;
	}

	void fireMissileRequest(bool toggle) {
		inputRequest.fireMissile = toggle;
	}

	void dropBombRequest() {
		inputRequest.dropBomb = true;
	}

	bool isShieldActive() {
		return playerShield->isActive();
	}

	bool isShieldVisible() const {
		return playerShield->isVisible();
	}

	uint getNumSmartBombs() const {
		return numSmartBombs;
	}

	uint getNumLives() const {
		return numLives;
	}

	uint getCurrentScore() const {
		return currentScore;
	}

	PowerUpType getCurrentPowerUp() const {
		return currentPowerUp;
	}

	// Overridden.
	void setActive(bool active);

	void resetInputRequests() {
		inputRequest.reset();
	}

	// Overridden.
	NovaVertex getExplosionOrigin() const {
		return this->getPositionWCS();
	}

	// Overridden.
	const NovaColor& getExplosionColor() const {
		return currentColor;
	}

	void youHit(Alien *alien);
	void youHit(Nugget *nugget);
	void checkCollision(Missile *missile);
	void increaseScore(Alien *alien);
	void update(float elapsedTime);
	void updateVelocity(double additionalHorizontalVelocity, double additionalVerticalVelocity);
	void draw();

private:
	PlayState* playState;
	InputRequest inputRequest;
	SpriteDefinition *shieldDefinition;
	PlayerShield *playerShield;
	uint numSmartBombs;
	bool autofireModeActive;
	float autofireTimer;
	uint numLives;
	uint currentScore;
	bool highScoreAchived;
	float scoreMultiplier;
	PowerUpType currentPowerUp;
	float powerUpTimer;
	bool restartMusic;
};

#endif // __PLAYER_H
