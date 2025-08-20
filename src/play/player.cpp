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

#include "poly_nova.h"

#define DEFAULT_AUTOFIRE_INTERVAL 2
#define POWER_UP_AUTOFIRE_INTERVAL 1
#define DEFAULT_NUMBER_OF_SMART_BOMBS 3
#define MAX_NUMBER_OF_SMART_BOMBS 9
#define DEFAULT_NUMBER_LIVES 3
#define MAX_NUMBER_LIVES 9
#define POWER_UP_DURATION 150
#define DEFAULT_AWARD_SCORE 50000
#define AWARD_SCORE_MULTIPLIER 3
#define DEFAULT_SHIP_VELOCITY 4.0
#define POWER_UP_SHIP_VELOCITY 6.0
#define MAX_SHIP_VELOCITY 6.0
#define DECELERATION_FACTOR 1.5
#define NUMBER_OF_POWER_UP_TYPES 4
#define DEFAULT_SCORE_MULTIPLIER 1
#define SCORE_MULTIPLIER_INCREMENT 1

static PowerUpType powerUpTypes[NUMBER_OF_POWER_UP_TYPES] = { NO_POWER_UP_ASSIGNED };

Player::Player(PlayState *playState) : Sprite(playState) {
	this->playState = playState;
	this->setSpriteDefinition("player");
	this->setSpriteColor(NovaColor(255, 255, 255));
	this->setExplosionColor(this->getSpriteColor());
	this->moveTo(NovaVertex(0, 0, 0));

	// Create the shield definition.
	shieldDefinition = new SpriteDefinition();

	// Create the vertex data.
	int numPoints = 72; // Number of points to calculate along the circle
	float radius = 6.0; // Radius of the circle

	shieldDefinition->allocateVertices(numPoints);

	// Loop through each point around the circle
	for (int i = 0; i < numPoints; i++) {
		// Calculate angle in radians for this point
		float angle = (2 * M_PI * i) / numPoints;

		// Calculate the x and y coordinates using cosine and sine
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		shieldDefinition->setVertex(i, x, y, 0);
	}

	// Create the line data.
	shieldDefinition->allocateLines(numPoints);
	int startVertex = 0;
	int endVertex = 1;
	for (int i = 0; i < numPoints; i++) {
		shieldDefinition->setLineVertexMapping(i, startVertex++, endVertex);

		if (endVertex < (numPoints - 1)) {
			endVertex++;
		} else {
			// Close the circle.
			endVertex = 0;
		}
	}

	shieldDefinition->calculateBoundingSphere();

	// Create the shield.
	playerShield = new PlayerShield(playState, shieldDefinition);

	// Set the default property values.
	fireMissileRequested = false;
	dropBombRequested = false;
	numSmartBombs = DEFAULT_NUMBER_OF_SMART_BOMBS;
	autofireModeActive = false;
	autofireTimer = 0;
	numLives = DEFAULT_NUMBER_LIVES;
	currentScore = 0;
	highScore = g_worldManager->getHighScoreHandler()->getHighScore();
	highScoreAchived = false;
	awardScore = DEFAULT_AWARD_SCORE;
	scoreMultiplier = DEFAULT_SCORE_MULTIPLIER;
	currentPowerUp = NO_POWER_UP_ASSIGNED;
	powerUpTimer = 0;
	thrustSoundChannel = 0;
    restartMusic = false;
    
	// Set the power-ups.
	powerUpTypes[0] = INCREASED_SPEED_POWER_UP;
	powerUpTypes[1] = INCREASED_FIRE_RATE_POWER_UP;
	powerUpTypes[2] = MULTISHOT_POWER_UP;
	powerUpTypes[3] = HOMING_MISSILE_POWER_UP;
}

Player::~Player() {
	if (playerShield) {
		delete playerShield;
		playerShield = NULL;
	}

	if (shieldDefinition) {
		delete shieldDefinition;
		shieldDefinition = NULL;
	}
}

void Player::setActive(bool active) {
	// Base processing.
	Sprite::setActive(active);

	if (active) {
		// Make some sound.
		g_worldManager->startSound(PLAYER_SPAWN);

		// Only start play music if the player stopped the music when they died.
		if (restartMusic) {
			g_worldManager->startMusic();
			restartMusic = false;
		}

		// Create a temporary shield.
		playerShield->setActive(true);
	} else {
		// Player has been destroyed.
		this->setVelocityFromComponents(0, 0);
		g_worldManager->stopMusic();
		playState->getExplosionController()->createExplosion(this);

		numLives--;
		if (!numLives) {
			// Player is dead and not coming back.
			g_worldManager->getHighScoreHandler()->addScore(currentScore);
			g_worldManager->startSound(GAME_OVER);
			playState->getHudController()->getGameOverPanel()->setVisible(true);
		} else {
			// Player died but will be reborn....
			restartMusic = true;
		}

		// Reset score multiplier.
		scoreMultiplier = DEFAULT_SCORE_MULTIPLIER;

		// Remove any assigned power-up.
		currentPowerUp = NO_POWER_UP_ASSIGNED;
		this->setSpriteColor(NovaColor(255, 255, 255));

		// Update the stats panel.
		playState->getHudController()->getPlayerStatsPanel()->setDirty(true);
	}
}

void Player::youHit(Alien *alien) {
	// The player has collided with an alien.
	if (!playerShield->isActive()) {
		// Player destroyed.
		this->setActive(false);
	} else {
		// An alien collided with the player's shield.
		if (alien->isActive()) {
			// The alien wasn't destroyed by the collision, play a collision sound.
			g_worldManager->startSound(PLAYER_SHIELD_HIT_ENEMY);
		}
	}
}

void Player::youHit(Nugget *nugget) {
	if (nugget->getNuggetType() == Nugget::MULTIPLIER) {
		// Player just picked up a nugget, increase the score multiplier.
		g_worldManager->startSound(PICKUP_MULTIPLIER);
		scoreMultiplier += SCORE_MULTIPLIER_INCREMENT;
	} else {
		// Player just picked up a nugget, randomly choose a new power-up.
		g_worldManager->startSound(PICKUP_POWER_UP);
		currentPowerUp = powerUpTypes[int_rand(0, 4)];
		this->setSpriteColor(NovaColor(253, 243, 2));

		// Reset.
		powerUpTimer = 0;
		thrustSoundChannel = 0;
	}
}

void Player::checkCollision(Missile *missile) {
	// See if the player has just hit this missile.
	NovaVertex between = (missile->getPositionWCS() - this->getPositionWCS());
	if (between.magnitude() < (missile->getBoundingSphere() + this->getBoundingSphere())) {
		// This player was hit by the alien's missile.
		if (!playerShield->isActive()) {
			// Player destroyed.
			this->setActive(false);
		}

		// Missile has been destroyed.
		playState->getMissileController()->deactivate(missile);
	}
}

void Player::increaseScore(Alien *alien) {
	// Only increase the score if the player is still alive.
	if (this->isActive()) {
		uint pointsAwarded = 0;

		// The alien was destroyed, award points depending on the value of the various aliens.
		switch (alien->getAlienType()) {
		case Alien::ROCKET_SHIP:
			pointsAwarded = 1000;
			break;

		case Alien::FLYING_SAUCER:
			pointsAwarded = 3000;
			break;

		case Alien::PLAYER_CLONE:
			pointsAwarded = 6000;
			break;

		case Alien::BLACK_HOLE:
			pointsAwarded = 12000;
			break;

		case Alien::MINI_GATE:
			pointsAwarded = 24000;
			break;

		case Alien::SNAKE:
			pointsAwarded = 48000;
			break;

		case Alien::JELLY:
			pointsAwarded = 58000;
			break;

		case Alien::CRUSHER:
			pointsAwarded = 68000;
			break;

		default:
			// Standard aliens.
			pointsAwarded = 100;
		}

		currentScore += (pointsAwarded * scoreMultiplier);
		
		// See if the current score is higher than the high-score, if it is then we need to update it and play SFX.
		if (currentScore > highScore) {
			if (!highScoreAchived) {
				g_worldManager->startSound(HIGH_SCORE_ACHIEVED);
				highScoreAchived = true;
			}

			highScore = currentScore;

			// Notify the high score panel that it needs to be updated.
			playState->getHudController()->getHighScorePanel()->setDirty(true);
		}

		// Award extra lives and bombs when you get to certain score thresholds.
		if (currentScore > awardScore) {
			// Randomly assign either an extra life or an extra smart bomb.
			if (int_rand(0, 3) == 2) {
				// Award extra life (1 in 3 chance).
				if (numLives < MAX_NUMBER_LIVES) {
					g_worldManager->startSound(AWARDED_EXTRA_LIFE);
					numLives++;
				}
			} else {
				// Award extra smart bomb (2 in 3 chance).
				if (numSmartBombs < MAX_NUMBER_OF_SMART_BOMBS) {
					g_worldManager->startSound(AWARDED_SMARTBOMB);
					numSmartBombs++;
				}
			}
			
			// Update the stats panel.
			playState->getHudController()->getPlayerStatsPanel()->setDirty(true);

			// Make it harder to achieve the next award.
			awardScore = (awardScore * AWARD_SCORE_MULTIPLIER);
		}

		// Notify the current score panel that it needs to be updated.
		playState->getHudController()->getCurrentScorePanel()->setDirty(true);
	}
}

void Player::update(float elapsedTime) {
	NovaVertex originalShipPosition = this->getPositionWCS();

	// See if the power-up has expired.
	if (currentPowerUp != NO_POWER_UP_ASSIGNED) {
		powerUpTimer += elapsedTime;

		if (powerUpTimer > POWER_UP_DURATION) {
			currentPowerUp = NO_POWER_UP_ASSIGNED;
			this->setSpriteColor(NovaColor(255, 255, 255));
		}
	}

	if (inputRequest.up || inputRequest.down || inputRequest.left || inputRequest.right) {
		// Movement has been requested.
		float direction = 0;

		if (inputRequest.up || inputRequest.down) {
			if (inputRequest.up) {
				if (inputRequest.left) {
					direction = UP_AND_LEFT;
				} else if (inputRequest.right) {
					direction = UP_AND_RIGHT;
				} else {
					direction = UP;
				}
			} else {
				// Down.
				if (inputRequest.left) {
					direction = DOWN_AND_LEFT;
				} else if (inputRequest.right) {
					direction = DOWN_AND_RIGHT;
				} else {
					direction = DOWN;
				}
			}
		} else {
			// Not up or down.
			if (inputRequest.left) {
				direction = LEFT;
			} else {
				direction = RIGHT;
			}
		}

		// Set the new direction (this will overwrite any current velocity).
		if (currentPowerUp == INCREASED_SPEED_POWER_UP) {
			this->setVelocityFromDirection(direction, POWER_UP_SHIP_VELOCITY);
			bool restartSound = false;

			if (!thrustSoundChannel) {
				restartSound = true;
			} else if (!g_worldManager->isSoundPlaying(thrustSoundChannel)) {
				restartSound = true;
			}

			if (restartSound) {
				thrustSoundChannel = g_worldManager->startSound(PLAYER_THRUST_LOOP);
			}
		} else {
			this->setVelocityFromDirection(direction, DEFAULT_SHIP_VELOCITY);
		}

		// Rotate the sprite to match the direction of travel.
		this->setFacingTowardsDirection(direction);
	} else {
	    // Reduce any current velocity.
    	if (horizontalVelocity != 0) {
    	    if (horizontalVelocity > 0) {
    	        horizontalVelocity -= (DECELERATION_FACTOR * elapsedTime);
    	        if (horizontalVelocity < 0) {
    	            horizontalVelocity = 0;
    	        }
    	    } else {
    	        horizontalVelocity += (DECELERATION_FACTOR * elapsedTime);
    	        if (horizontalVelocity > 0) {
    	            horizontalVelocity = 0;
    	        }
    	    }
    	}
    
    	if (verticalVelocity != 0) {
    	    if (verticalVelocity > 0) {
    	    	verticalVelocity -= (DECELERATION_FACTOR * elapsedTime);
    	        if (verticalVelocity < 0) {
    	        	verticalVelocity = 0;
    	        }
    	    } else {
    	    	verticalVelocity += (DECELERATION_FACTOR * elapsedTime);
    	        if (verticalVelocity > 0) {
    	        	verticalVelocity = 0;
    	        }
    	    }
    	}
	}

	// See if we need to add gravitational pull from any aliens.
	if (!playerShield->isActive()) {
		playState->getAlienController()->applyGravitionalPull(this);
	}

	if (this->getTotalVelocity()) {
		// We are moving, apply the velocity.
		NovaVertex newShipPosition = originalShipPosition;
		newShipPosition.x += (horizontalVelocity * elapsedTime);
		newShipPosition.y += (verticalVelocity * elapsedTime);
		this->moveTo(newShipPosition);

		// Check for collision with the play-area borders first (in case we need to update the ship position).
		bool borderViolated = false;

		NovaVertex borderPosition = playState->getPlayAreaController()->getBorderPositionWCS(LEFT_BORDER);
		if ((newShipPosition.x - this->getBoundingSphere()) < borderPosition.x) {
			borderViolated = true;
			newShipPosition.x = (borderPosition.x + this->getBoundingSphere());
		}

		borderPosition = playState->getPlayAreaController()->getBorderPositionWCS(TOP_BORDER);
		if ((newShipPosition.y + this->getBoundingSphere()) > borderPosition.y) {
			borderViolated = true;
			newShipPosition.y = (borderPosition.y - this->getBoundingSphere());
		}

		borderPosition = playState->getPlayAreaController()->getBorderPositionWCS(RIGHT_BORDER);
		if ((newShipPosition.x + this->getBoundingSphere()) > borderPosition.x) {
			borderViolated = true;
			newShipPosition.x = (borderPosition.x - this->getBoundingSphere());
		}

		borderPosition = playState->getPlayAreaController()->getBorderPositionWCS(BOTTOM_BORDER);
		if ((newShipPosition.y - this->getBoundingSphere()) < borderPosition.y) {
			borderViolated = true;
			newShipPosition.y = (borderPosition.y + this->getBoundingSphere());
		}

		// Only update the player position if it has changed.
		if (borderViolated) {
			// Border collision occurred, update with the adjusted player position.
			this->moveTo(newShipPosition);
			this->setVelocityFromComponents(0, 0);
			g_worldManager->startSound(PLAYER_HIT_WALL);
		}

		// See if we should sync the playState->getCamera() to the player's movement.
		float horizontalMovement = (newShipPosition.x - originalShipPosition.x);
		float verticalMovement = (newShipPosition.y - originalShipPosition.y);

		// Assume that we will sync until proven otherwise.
		bool syncHorizontalMovement = true;
		bool syncVerticalMovement = true;

		if (horizontalMovement > 0) {
			// Don't sync if the border is currently visible.
			if (playState->getPlayAreaController()->isVisible(RIGHT_BORDER)) {
				syncHorizontalMovement = false;
			} else {
				// Don't sync if the player is still in the left half of the screen.
				NovaVertex locationCCS = (newShipPosition * playState->getCamera()->getWorldToCameraMatrix());
				if (locationCCS.x < 0) {
					syncHorizontalMovement = false;
				}
			}
		}

		if (horizontalMovement < 0) {
			// Don't sync if the border is currently visible.
			if (playState->getPlayAreaController()->isVisible(LEFT_BORDER)) {
				syncHorizontalMovement = false;
			} else {
				// Don't sync if the player is still in the right half of the screen.
				NovaVertex locationCCS = (newShipPosition * playState->getCamera()->getWorldToCameraMatrix());
				if (locationCCS.x > 0) {
					syncHorizontalMovement = false;
				}
			}
		}

		if (verticalMovement > 0) {
			// Don't sync if the border is currently visible.
			if (playState->getPlayAreaController()->isVisible(TOP_BORDER)) {
				syncVerticalMovement = false;
			} else {
				// Don't sync if the player is still in the bottom half of the screen.
				NovaVertex locationCCS = (newShipPosition * playState->getCamera()->getWorldToCameraMatrix());
				if (locationCCS.y < 0) {
					syncVerticalMovement = false;
				}
			}
		}

		if (verticalMovement < 0) {
			// Don't sync if the border is currently visible.
			if (playState->getPlayAreaController()->isVisible(BOTTOM_BORDER)) {
				syncVerticalMovement = false;
			} else {
				// Don't sync if the player is still in the top half of the screen.
				NovaVertex locationCCS = (newShipPosition * playState->getCamera()->getWorldToCameraMatrix());
				if (locationCCS.y > 0) {
					syncVerticalMovement = false;
				}
			}
		}

		if (syncHorizontalMovement || syncVerticalMovement) {
			NovaVertex newCameraPosition = playState->getCamera()->getPositionWCS();

			if (syncHorizontalMovement) {
				newCameraPosition.x += horizontalMovement;
			}

			if (syncVerticalMovement) {
				newCameraPosition.y += verticalMovement;
			}

			playState->getCamera()->moveTo(newCameraPosition);

			// Update our camera matrix so that objects can be transformed into the CCS.
			playState->getCamera()->calculateWorldToCameraMatrix();
		}
	}

	if (fireMissileRequested) {
		if (autofireModeActive) {
			autofireTimer += elapsedTime;
			float autofireInterval = (currentPowerUp == INCREASED_FIRE_RATE_POWER_UP) ? POWER_UP_AUTOFIRE_INTERVAL : DEFAULT_AUTOFIRE_INTERVAL;
			if (autofireTimer >= autofireInterval) {
				playState->getMissileController()->launchMissile(this);

				// Reset.
				autofireTimer = 0;
			}
		} else {
			// First time.
			playState->getMissileController()->launchMissile(this);
			autofireModeActive = true;
		}
	} else {
		// Reset.
		autofireModeActive = false;
		autofireTimer = 0;
	}

	if (dropBombRequested) {
		if (numSmartBombs > 0) {
			NuggetSpawnMode savedMode = playState->getNuggetController()->setSpawnMode(NEVER_SPAWN_NUGGETS);
			playState->getAlienController()->destroyAliens();
			playState->getNuggetController()->setSpawnMode(savedMode);
			playState->getPlayAreaController()->shakeGrid();
			g_worldManager->startSound(FIRE_SMARTBOMB_LOW);
			numSmartBombs--;

			// Update the stats panel.
			playState->getHudController()->getPlayerStatsPanel()->setDirty(true);
		}

		// Reset.
		dropBombRequested = false;
	}

	if (playerShield->isActive()) {
		playerShield->update(elapsedTime);
	}
}


void Player::updateVelocity(double additionalHorizontalVelocity, double additionalVerticalVelocity) {
    if ((additionalHorizontalVelocity != 0) || (additionalVerticalVelocity != 0)) {
        horizontalVelocity += additionalHorizontalVelocity;
        verticalVelocity += additionalVerticalVelocity;

        // Calculate the magnitude of the velocity vector.
        double totalVelocity = this->getTotalVelocity();

        // Clamp the total velocity to the maximum velocity.
        if (totalVelocity > MAX_SHIP_VELOCITY) {
            double scale = MAX_SHIP_VELOCITY / totalVelocity;
            horizontalVelocity *= scale;
            verticalVelocity *= scale;
        }
    }
}

void Player::draw() {
	// Do base class processing.
	Sprite::draw();

	// Also draw the shield.
	if (playerShield->isVisible()) {
		playerShield->moveTo(this->getPositionWCS());
		playerShield->draw();
	}
}

