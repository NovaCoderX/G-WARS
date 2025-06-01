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

#define INITIAL_SPAWN_DELAY 100
#define MIN_STANDARD_ALIEN_SPAWN_INTERVAL 5
#define MAX_STANDARD_ALIEN_SPAWN_INTERVAL 50
#define SPECIAL_ALIEN_SPAWN_INTERVAL 300

#define WAVE_INTENSITY_INCREMENT_DURATION 300
#define WAVE_INTENSITY_DECREMENT_DURATION 300
#define WAVE_INTENSITY_MIDPOINT_INITIAL_DURATION 30
#define WAVE_INTENSITY_MIDPOINT_DURATION_INCREMENT 10

#define NUMBER_OF_BOUNCE_ALIENS 10
#define NUMBER_OF_CHASE_ALIENS 8
#define NUMBER_OF_ATTACK_ALIENS 6

enum GameStage {
	STAGE_ZERO = 0, STAGE_A = 1, STAGE_B = 2, STAGE_C = 3, BOSS_STAGE = 4
};

enum SpawnWave {
	BOUNCE_WAVE = 0, CHASE_WAVE = 1, ATTACK_WAVE = 2
};

enum WaveTypes {
	FIRST_WAVE_TYPE = 0, SECOND_WAVE_TYPE = 1, THIRD_WAVE_TYPE = 2, FOURTH_WAVE_TYPE = 3
};

enum SpawnIntensity {
	INCREASING_SPAWN_INTENSITY = 0,
	DECREASING_SPAWN_INTENSITY = 1,
	MIDPOINT_SPAWN_INTENSITY = 2
};

static AlienType spawnMatrix[3][4];
static AlienType specialAlienTypes[8];

AlienController::AlienController(PlayState* playState) {
	this->playState = playState;

	// Standard aliens.
	alienListHead = NULL;

	// Special aliens.
	rocketShip = NULL;
	flyingSaucer = NULL;
	playerClone = NULL;
	blackHole = NULL;
	miniGate = NULL;

	// Bosses.
	snake = NULL;

	// Spawn logic.
	spawnLogic.currentGameStage = 0;
	spawnLogic.currentWave = 0;
	spawnLogic.intensity = 0;
	spawnLogic.currentIntervalStandard = 0;
	spawnLogic.currentIntervalSpecial = 0;
	spawnLogic.intervalIncrement = 0;
	spawnLogic.intervalDecrement = 0;
	spawnLogic.midPointStartTime = 0;
	spawnLogic.midPointDuration = 0;
	spawnLogic.lastSpawnedStandard = 0;
	spawnLogic.lastSpawnedSpecial = 0;
	spawnLogic.bossStageStartTime = 0;

	totalElapsedTime = 0;
}

AlienController::~AlienController() {
	for (Alien* alien : bounceCubes) {
		delete alien;
	}

	bounceCubes.clear();

	for (Alien* alien : bounceWanderers) {
		delete alien;
	}

	bounceWanderers.clear();

	for (Alien* alien : bounceStars) {
		delete alien;
	}

	bounceStars.clear();

	for (Alien* alien : bounceHexagons) {
		delete alien;
	}

	bounceHexagons.clear();

	for (Alien* alien : chaseRhombi) {
		delete alien;
	}

	chaseRhombi.clear();

	for (Alien* alien : chaseStars) {
		delete alien;
	}

	chaseStars.clear();

	for (Alien* alien : chaseShurikens) {
		delete alien;
	}

	chaseShurikens.clear();

	for (Alien* alien : chaseConcaves) {
		delete alien;
	}

	chaseConcaves.clear();

	for (Alien* alien : attackTanks) {
		delete alien;
	}

	attackTanks.clear();

	for (Alien* alien : attackShips) {
		delete alien;
	}

	attackShips.clear();

	for (Alien* alien : attackArtillery) {
		delete alien;
	}

	attackArtillery.clear();

	for (Alien* alien : attackNeutrons) {
		delete alien;
	}

	attackNeutrons.clear();

	if (rocketShip) {
		delete rocketShip;
		rocketShip = NULL;
	}

	if (flyingSaucer) {
		delete flyingSaucer;
		flyingSaucer = NULL;
	}

	if (playerClone) {
		delete playerClone;
		playerClone = NULL;
	}

	if (blackHole) {
		delete blackHole;
		blackHole = NULL;
	}

	if (miniGate) {
		delete miniGate;
		miniGate = NULL;
	}

	if (snake) {
		delete snake;
		snake = NULL;
	}
}

void AlienController::init() {
	for (int i = 0; i < NUMBER_OF_BOUNCE_ALIENS; i++) {
		bounceCubes.push_back(new BounceCube(playState));
		bounceWanderers.push_back(new BounceWanderer(playState));
		bounceStars.push_back(new BounceStar(playState));
		bounceHexagons.push_back(new BounceHexagon(playState));
	}

	for (int i = 0; i < NUMBER_OF_CHASE_ALIENS; i++) {
		chaseRhombi.push_back(new ChaseRhombus(playState));
		chaseStars.push_back(new ChaseStar(playState));
		chaseShurikens.push_back(new ChaseShuriken(playState));
		chaseConcaves.push_back(new ChaseConcave(playState));
	}

	for (int i = 0; i < NUMBER_OF_ATTACK_ALIENS; i++) {
		attackTanks.push_back(new AttackTank(playState));
		attackShips.push_back(new AttackShip(playState));
		attackArtillery.push_back(new AttackArtillery(playState));
		attackNeutrons.push_back(new AttackNeutron(playState));
	}

	// Special aliens.
	rocketShip = new RocketShip(playState);
	flyingSaucer = new FlyingSaucer(playState);
	playerClone = new PlayerClone(playState);
	blackHole = new BlackHole(playState);
	miniGate = new MiniGate(playState);

	// Boss aliens.
	snake = new Snake(playState);

	// Setup spawn logic.
	spawnMatrix[BOUNCE_WAVE][FIRST_WAVE_TYPE] = BOUNCE_CUBE;
	spawnMatrix[BOUNCE_WAVE][SECOND_WAVE_TYPE] = BOUNCE_WANDERER;
	spawnMatrix[BOUNCE_WAVE][THIRD_WAVE_TYPE] = BOUNCE_STAR;
	spawnMatrix[BOUNCE_WAVE][FOURTH_WAVE_TYPE] = BOUNCE_HEXAGON;

	spawnMatrix[CHASE_WAVE][FIRST_WAVE_TYPE] = CHASE_RHOMBUS;
	spawnMatrix[CHASE_WAVE][SECOND_WAVE_TYPE] = CHASE_STAR;
	spawnMatrix[CHASE_WAVE][THIRD_WAVE_TYPE] = CHASE_SHURIKEN;
	spawnMatrix[CHASE_WAVE][FOURTH_WAVE_TYPE] = CHASE_CONCAVE;

	spawnMatrix[ATTACK_WAVE][FIRST_WAVE_TYPE] = ATTACK_TANK;
	spawnMatrix[ATTACK_WAVE][SECOND_WAVE_TYPE] = ATTACK_SHIP;
	spawnMatrix[ATTACK_WAVE][THIRD_WAVE_TYPE] = ATTACK_ARTILLERY;
	spawnMatrix[ATTACK_WAVE][FOURTH_WAVE_TYPE] = ATTACK_NEUTRON;

	specialAlienTypes[0] = ROCKET_SHIP;
	specialAlienTypes[1] = ROCKET_SHIP;
	specialAlienTypes[2] = ROCKET_SHIP;
	specialAlienTypes[3] = FLYING_SAUCER;
	specialAlienTypes[4] = FLYING_SAUCER;
	specialAlienTypes[5] = PLAYER_CLONE;
	specialAlienTypes[6] = BLACK_HOLE;
	specialAlienTypes[7] = MINI_GATE;

	spawnLogic.intervalIncrement = (float) (MAX_STANDARD_ALIEN_SPAWN_INTERVAL - MIN_STANDARD_ALIEN_SPAWN_INTERVAL)
			/ WAVE_INTENSITY_INCREMENT_DURATION;
	spawnLogic.intervalDecrement = (float) (MAX_STANDARD_ALIEN_SPAWN_INTERVAL - MIN_STANDARD_ALIEN_SPAWN_INTERVAL)
			/ WAVE_INTENSITY_DECREMENT_DURATION;
	spawnLogic.midPointDuration = WAVE_INTENSITY_MIDPOINT_INITIAL_DURATION;
	spawnLogic.currentGameStage = STAGE_ZERO;

	totalElapsedTime = 0;
}

void AlienController::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// Update any active aliens.
	Alien* alien = alienListHead;
	while (alien) {
		alien->update(elapsedTime);

		alien = alien->nextInList;
	}

	// We need an initial delay before the first (spawn) stage starts.
	if (spawnLogic.currentGameStage == STAGE_ZERO) {
		if (totalElapsedTime > INITIAL_SPAWN_DELAY) {
			// Ready to rock and roll....
			spawnLogic.currentGameStage = STAGE_A;
			spawnLogic.currentWave = BOUNCE_WAVE;
			spawnLogic.currentWaveType[BOUNCE_WAVE] = FIRST_WAVE_TYPE;
			spawnLogic.currentWaveType[CHASE_WAVE] = FIRST_WAVE_TYPE;
			spawnLogic.currentWaveType[ATTACK_WAVE] = FIRST_WAVE_TYPE;
			spawnLogic.intensity = INCREASING_SPAWN_INTENSITY;
			spawnLogic.currentIntervalStandard = MAX_STANDARD_ALIEN_SPAWN_INTERVAL;
			spawnLogic.currentIntervalSpecial = SPECIAL_ALIEN_SPAWN_INTERVAL;
			
			// Start playing standard music.
		    g_worldManager->setMusicType(STANDARD_MUSIC);
		    g_worldManager->startMusic();
		}
	}

	// Spawn aliens.
	if (spawnLogic.currentGameStage > STAGE_ZERO && spawnLogic.currentGameStage < BOSS_STAGE) {
		// See if we need to create more standard aliens.
		if ((totalElapsedTime - spawnLogic.lastSpawnedStandard) >= spawnLogic.currentIntervalStandard) {
			// Ready to spawn another standard alien (if the player is active).
			if (playState->getPlayer()->isActive()) {
				spawnStandardAlien(spawnMatrix[spawnLogic.currentWave][spawnLogic.currentWaveType[spawnLogic.currentWave]]);

				// Store the time of the last spawn attempt (may not have actually spawned).
				spawnLogic.lastSpawnedStandard = totalElapsedTime;
			}
		}
		
		// See if we need to create more special aliens.
		if ((totalElapsedTime - spawnLogic.lastSpawnedSpecial) >= spawnLogic.currentIntervalSpecial) {
			// Ready to spawn another special alien (if the player is active).
			if (playState->getPlayer()->isActive()) {
				spawnSpecialAlien(specialAlienTypes[int_rand(0, 7)]);

				// Store the time of the last spawn attempt (may not have actually spawned).
				spawnLogic.lastSpawnedSpecial = totalElapsedTime;
			}
		}

		if (spawnLogic.intensity == INCREASING_SPAWN_INTENSITY) {
			// Build intensity by reducing the spawn frequency.
			spawnLogic.currentIntervalStandard -= (spawnLogic.intervalDecrement * elapsedTime);
			if (spawnLogic.currentIntervalStandard <= MIN_STANDARD_ALIEN_SPAWN_INTERVAL) {
				// Highest intensity reached.
				spawnLogic.currentIntervalStandard = MIN_STANDARD_ALIEN_SPAWN_INTERVAL;
				spawnLogic.intensity = MIDPOINT_SPAWN_INTENSITY;

				// Change the type of standard alien to spawn.
				spawnLogic.currentWaveType[spawnLogic.currentWave]++;
				if (spawnLogic.currentWaveType[spawnLogic.currentWave] > FOURTH_WAVE_TYPE) {
					spawnLogic.currentWaveType[spawnLogic.currentWave] = FIRST_WAVE_TYPE;
				}

				// Store the start time of the wave midpoint.
				spawnLogic.midPointStartTime = totalElapsedTime;
			}
		}

		if (spawnLogic.intensity == DECREASING_SPAWN_INTENSITY) {
			// Reduce intensity by increasing the spawn frequency.
			spawnLogic.currentIntervalStandard += (spawnLogic.intervalIncrement * elapsedTime);
			if (spawnLogic.currentIntervalStandard >= MAX_STANDARD_ALIEN_SPAWN_INTERVAL) {
				// Lowest intensity reached (this is the end of the current wave).
				spawnLogic.currentIntervalStandard = MAX_STANDARD_ALIEN_SPAWN_INTERVAL;
				spawnLogic.intensity = INCREASING_SPAWN_INTENSITY;

				if (spawnLogic.currentWave < ATTACK_WAVE) {
					// Move to the next wave of the current stage.
					spawnLogic.currentWave++;
				} else {
					// We are on the last wave of this stage, move to the next stage.
					spawnLogic.currentGameStage++;

					// Make each stage harder than the one before.
					spawnLogic.midPointDuration += WAVE_INTENSITY_MIDPOINT_DURATION_INCREMENT;

					// See if we have moved to the boss stage.
					if (spawnLogic.currentGameStage < BOSS_STAGE) {
						// Loop back to the first wave of the new stage.
						spawnLogic.currentWave = BOUNCE_WAVE;
					} else {
						// We have moved to the boss stage.
						g_worldManager->stopMusic();
						g_worldManager->setMusicType(NO_MUSIC);
						spawnLogic.bossStageStartTime = totalElapsedTime;
					}
				}
			}
		}

		// Maintain the highest spawn frequency for a specific duration (depending on the current difficulty).
		if (spawnLogic.intensity == MIDPOINT_SPAWN_INTENSITY) {
			if ((totalElapsedTime - spawnLogic.midPointStartTime) > spawnLogic.midPointDuration) {
				spawnLogic.intensity = DECREASING_SPAWN_INTENSITY;
			}
		}
	}

	// We handle the boss stage differently.
	if (spawnLogic.currentGameStage == BOSS_STAGE) {
		// See if we need to spawn the boss alien.
		if (!snake->isActive()) {
			if ((totalElapsedTime - spawnLogic.bossStageStartTime) > INITIAL_SPAWN_DELAY) {
				// Ready to spawn if all other aliens have been destroyed and the player is active.
				if ((!alienListHead) && (playState->getPlayer()->isActive())) {
					g_worldManager->setMusicType(BOSS_MUSIC);
		            g_worldManager->startMusic();
		            spawnBossAlien();
				}
			}
		}
	}
}

void AlienController::checkCollisions() {
	Player* player = playState->getPlayer();

	// Check for collisions.
	Alien* alien = alienListHead;
	while (alien && player->isActive()) {
		if (alien->isVisible()) {
			// See if this alien has just hit the player.
			alien->checkCollision(player);
		}

		alien = alien->nextInList;
	}
}

void AlienController::draw() {
	// Draw the visible objects.
	Alien* alien = alienListHead;
	while (alien) {
		if (alien->isVisible()) {
			alien->draw();
		}

		alien = alien->nextInList;
	}
}

void AlienController::deactivate(Alien* alien) {
	this->removeFromList(alien);

	if (alien->getAlienType() == SNAKE) {
		// Loop back to the starting stage (restart the game).
		spawnLogic.currentGameStage = STAGE_ZERO;
		
		// Stop the boss music.
		g_worldManager->stopMusic();

		// Make the next game more interesting.
		playState->getNuggetController()->setSpawnMode(ALWAYS_SPAWN_NUGGETS);

		// Reset the timers.
		spawnLogic.lastSpawnedStandard = 0;
		spawnLogic.lastSpawnedSpecial = 0;
		totalElapsedTime = 0;
	}
}

void AlienController::destroyAllLife() {
	// Smart bombs do not have any effect on boss aliens.
	if (spawnLogic.currentGameStage != BOSS_STAGE) {
		// Destroy all life.
		Alien* alien = alienListHead;
		while (alien) {
			this->deactivate(alien);

			alien = alien->nextInList;
		}
	}
}

Alien* AlienController::findBestTarget(Missile* missile) {
	static NovaVertex missilePosition;
	static NovaVertex alienPosition;

	Alien* target = NULL;

	// Homing missiles will not work in the boss battle.
	if (spawnLogic.currentGameStage != BOSS_STAGE) {
		missilePosition = missile->getPositionWCS();
		float closest = 0;

		Alien* alien = alienListHead;
		while (alien) {
			// Only consider targets that the missile is heading towards.
			bool candidate = false;

			alienPosition = alien->getPositionWCS();

			if ((alienPosition.x - missilePosition.x) > 0) {
				if (missile->getHorizontalVelocity() > 0) {
					candidate = true;
				}
			}

			if ((alienPosition.x - missilePosition.x) < 0) {
				if (missile->getHorizontalVelocity() < 0) {
					candidate = true;
				}
			}

			if (candidate) {
				// Also need to check vertical velocity.
				candidate = false;

				if ((alienPosition.y - missilePosition.y) > 0) {
					if (missile->getVerticalVelocity() > 0) {
						candidate = true;
					}
				}

				if ((alienPosition.y - missilePosition.y) < 0) {
					if (missile->getVerticalVelocity() < 0) {
						candidate = true;
					}
				}
			}

			if (candidate) {
				// If the missile is heading towards the alien, see if this closest one so far.
				float between = (alienPosition - missilePosition).magnitude();
				if ((!target) || (between < closest)) {
					// We found a closer alien (a better target).
					target = alien;

					// Store.
					closest = between;
				}
			}

			alien = alien->nextInList;
		}
	}

	return target;
}

void AlienController::applyGravitionalPull(Player* player) {
    if (blackHole->isActive()) {
		blackHole->applyGravitionalPull(player);
	}
}

bool AlienController::isWithinBlackHole(const NovaVertex &positionCCS) {
	bool within = false;
	
	if (blackHole->isActive() && blackHole->isVisible()) {
		NovaVertex between = (positionCCS - blackHole->getPositionCCS());
		if (between.magnitude() < blackHole->getBoundingSphere()) {
			within = true;
		}
	}
	
	return within;
}

void AlienController::addToList(Alien* alien) {
	/*
	 ** If no objects have been inserted into the world yet
	 ** then make this object the head of the object list.
	 */
	if (!alienListHead) {
		alienListHead = alien;
		alienListHead->priorInList = NULL;
		alienListHead->nextInList = NULL;
	} else {
		/*
		 ** Otherwise find the last object in the world and then
		 ** set the passed object as the new last object in the
		 ** world.
		 */
		Alien* last_in_list = alienListHead;

		/*
		 ** Find the last object in the world
		 */
		while (last_in_list->nextInList) {
			/*
			 ** See if the passed object has already been inserted
			 */
			if (last_in_list == alien)
				return;

			last_in_list = last_in_list->nextInList;
		}

		/*
		 ** Adjust pointers
		 */
		last_in_list->nextInList = alien;
		alien->priorInList = last_in_list;
		alien->nextInList = NULL;
	}

	// Object is now part of the world (will be drawn).
	alien->setActive(true);
}

void AlienController::removeFromList(Alien* alien) {
	Alien* previousObj = alien->priorInList;
	Alien* nextObj = alien->nextInList;

	// Have to treat list head removal differently.
	if (alien == alienListHead) {
		alienListHead = nextObj;	// Assign new list head.
		if (alienListHead)
			alienListHead->priorInList = NULL;
	} else {
		if (previousObj)
			previousObj->nextInList = nextObj;

		// Set prior reference of the next object if there is one.
		if (nextObj)
			nextObj->priorInList = previousObj;
	}

	// Object is no longer part of the world (will not be drawn).
	alien->setActive(false);
}

void AlienController::spawnHelper(Alien* alien, float miniumVelocity, float maximumVelocity) {
	static bool verticalSwitch = true;
	double horizontalVelocity = 0;
	double verticalVelocity = 0;

	// Find out which spawn zone we should be using (based on the players current location).
	if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
		// Spawn the alien in the right zone.
		alien->moveTo(RIGHT_SPAWN_POINT_X, RIGHT_SPAWN_POINT_Y, 0);
		horizontalVelocity = -float_rand(miniumVelocity, maximumVelocity);

		if (verticalSwitch) {
			verticalVelocity = float_rand(miniumVelocity, maximumVelocity);
		} else {
			verticalVelocity = -float_rand(miniumVelocity, maximumVelocity);
		}

		alien->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
	} else {
		// Spawn the alien in the left zone.
		alien->moveTo(LEFT_SPAWN_POINT_X, LEFT_SPAWN_POINT_Y, 0);
		horizontalVelocity = float_rand(miniumVelocity, maximumVelocity);

		if (verticalSwitch) {
			verticalVelocity = float_rand(miniumVelocity, maximumVelocity);
		} else {
			verticalVelocity = -float_rand(miniumVelocity, maximumVelocity);
		}

		alien->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
	}

	verticalSwitch = (!verticalSwitch);

	// Then add it to the processing loop.
	this->addToList(alien);
}

void AlienController::spawnStandardAlien(AlienType type) {
	switch (type) {
	case BOUNCE_CUBE:
		spawnBounceCube();
		break;

	case BOUNCE_WANDERER:
		spawnBounceWanderer();
		break;

	case BOUNCE_STAR:
		spawnBounceStar();
		break;

	case BOUNCE_HEXAGON:
		spawnBounceHexagon();
		break;

	case CHASE_RHOMBUS:
		spawnChaseRhombus();
		break;

	case CHASE_STAR:
		spawnChaseStar();
		break;

	case CHASE_SHURIKEN:
		spawnChaseShuriken();
		break;

	case CHASE_CONCAVE:
		spawnChaseConcave();
		break;

	case ATTACK_TANK:
		spawnAttackTank();
		break;

	case ATTACK_SHIP:
		spawnAttackShip();
		break;

	case ATTACK_ARTILLERY:
		spawnAttackArtillery();
		break;

	default: // ATTACK_NEUTRON
		spawnAttackNeutron();
	}
}

void AlienController::spawnBounceCube() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < NUMBER_OF_BOUNCE_ALIENS; i++) {
		if (!bounceCubes[i]->isActive()) {
			alien = bounceCubes[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 3, 6);
	}
}

void AlienController::spawnBounceWanderer() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < bounceWanderers.size(); i++) {
		if (!bounceWanderers[i]->isActive()) {
			alien = bounceWanderers[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 3, 6);
	}
}

void AlienController::spawnBounceStar() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < bounceStars.size(); i++) {
		if (!bounceStars[i]->isActive()) {
			alien = bounceStars[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 3, 6);
	}
}

void AlienController::spawnBounceHexagon() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < bounceHexagons.size(); i++) {
		if (!bounceHexagons[i]->isActive()) {
			alien = bounceHexagons[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 3, 6);
	}
}

void AlienController::spawnChaseRhombus() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < chaseRhombi.size(); i++) {
		if (!chaseRhombi[i]->isActive()) {
			alien = chaseRhombi[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnChaseStar() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < chaseStars.size(); i++) {
		if (!chaseStars[i]->isActive()) {
			alien = chaseStars[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnChaseShuriken() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < chaseShurikens.size(); i++) {
		if (!chaseShurikens[i]->isActive()) {
			alien = chaseShurikens[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnChaseConcave() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < chaseConcaves.size(); i++) {
		if (!chaseConcaves[i]->isActive()) {
			alien = chaseConcaves[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnAttackTank() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < attackTanks.size(); i++) {
		if (!attackTanks[i]->isActive()) {
			alien = attackTanks[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnAttackShip() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < attackShips.size(); i++) {
		if (!attackShips[i]->isActive()) {
			alien = attackShips[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnAttackArtillery() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < attackArtillery.size(); i++) {
		if (!attackArtillery[i]->isActive()) {
			alien = attackArtillery[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnAttackNeutron() {
	Alien* alien = NULL;

	// Find an empty slot.
	for (uint i = 0; i < attackNeutrons.size(); i++) {
		if (!attackNeutrons[i]->isActive()) {
			alien = attackNeutrons[i];
			break;
		}
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnSpecialAlien(AlienType type) {
	switch (type) {
	case ROCKET_SHIP:
		spawnRocketShip();
		break;

	case FLYING_SAUCER:
		spawnFlyingSaucer();
		break;

	case PLAYER_CLONE:
		spawnPlayerClone();
		break;

	case BLACK_HOLE:
		spawnBlackHole();
		break;

	default: // SMALL_GATE
		spawnMiniGate();
	}
}

void AlienController::spawnRocketShip() {
	RocketShip *alien = NULL;

	if (!rocketShip->isActive()) {
		alien = rocketShip;
	}

	// Setup initial position, direction and velocity of the new alien.
	if (alien) {
		float miniumVelocity = 7;
		float maximumVelocity = 14;

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			alien->moveTo(RIGHT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			alien->setVelocityFromComponents(-float_rand(miniumVelocity, maximumVelocity), 0);

			// Rotate the sprite to match the direction of travel.
			alien->setFacingTowardsDirection(LEFT);
		} else {
			// Spawn the alien in the left zone.
			alien->moveTo(LEFT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			alien->setVelocityFromComponents(float_rand(miniumVelocity, maximumVelocity), 0);

			// Rotate the sprite to match the direction of travel.
			alien->setFacingTowardsDirection(RIGHT);
		}

		// Then add it to the processing loop.
		this->addToList(alien);
	}
}

void AlienController::spawnFlyingSaucer() {
	FlyingSaucer *alien = NULL;

	if (!flyingSaucer->isActive()) {
		alien = flyingSaucer;
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnPlayerClone() {
	PlayerClone *alien = NULL;

	if (!playerClone->isActive()) {
		alien = playerClone;
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 2, 4);
	}
}

void AlienController::spawnBlackHole() {
	BlackHole *alien = NULL;

	if (!blackHole->isActive()) {
		alien = blackHole;
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 0.1, 0.3);
	}
}

void AlienController::spawnMiniGate() {
	MiniGate *alien = NULL;

	if (!miniGate->isActive()) {
		alien = miniGate;
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 1, 2);
	}
}

void AlienController::spawnBossAlien() {
	Snake *alien = NULL;

	// We currently only have a single boss alien.
	if (!snake->isActive()) {
		alien = snake;
	}

	if (alien) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(alien, 6, 9);
	}
}



