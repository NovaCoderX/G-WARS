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

#define STAGE_START_DELAY 100
#define MIN_STANDARD_ALIEN_SPAWN_INTERVAL 5
#define MAX_STANDARD_ALIEN_SPAWN_INTERVAL 50
#define SPECIAL_ALIEN_SPAWN_INTERVAL 300

#define WAVE_INTENSITY_INCREMENT_DURATION 300
#define WAVE_INTENSITY_DECREMENT_DURATION 300
#define WAVE_INTENSITY_MIDPOINT_INITIAL_DURATION 50
#define WAVE_INTENSITY_MIDPOINT_DURATION_INCREMENT 10

#define NUMBER_OF_BOUNCE_ALIENS 12
#define NUMBER_OF_CHASE_ALIENS 9
#define NUMBER_OF_ATTACK_ALIENS 6

enum GameStage {
	SPAWN_STAGE_STARTUP = 0,
	SPAWN_STAGE = 1,
	BOSS_STAGE_STARTUP = 2,
	BOSS_STAGE = 3
};

enum SpawnWave {
	FIRST_WAVE = 0, SECOND_WAVE = 1, THIRD_WAVE = 2, LAST_WAVE = 3
};

enum SpawnAlien {
	BOUNCE_ALIEN = 0, CHASE_ALIEN = 1, ATTACK_ALIEN = 2
};

enum SpawnIntensity {
	INCREASING_SPAWN_INTENSITY = 0,
	MIDPOINT_SPAWN_INTENSITY = 1,
	DECREASING_SPAWN_INTENSITY = 2
};

enum BossType {
	FIRST_BOSS = 0, SECOND_BOSS = 1, LAST_BOSS = 2
};

static Alien::AlienType spawnMatrix[3][4];
static Alien::AlienType specialAliens[8];
static Alien::AlienType bossAliens[3];

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
	jelly = NULL;
	crusher = NULL;

	currentGameStage = 0;
	currentBossType = 0;

	// Spawn logic.
	spawnLogic.currentAlienType = 0;
	spawnLogic.intensity = 0;
	spawnLogic.currentIntervalStandard = 0;
	spawnLogic.currentIntervalSpecial = 0;
	spawnLogic.intervalIncrement = 0;
	spawnLogic.intervalDecrement = 0;
	spawnLogic.midPointStartTime = 0;
	spawnLogic.midPointDuration = 0;
	spawnLogic.lastSpawnedStandard = 0;
	spawnLogic.lastSpawnedSpecial = 0;

	// Timers.
	stageStartDelay = 0;
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

	if (jelly) {
		delete jelly;
		jelly = NULL;
	}

	if (crusher) {
		delete crusher;
		crusher = NULL;
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
	jelly = new Jelly(playState);
	crusher = new Crusher(playState);

	// First and second waves of standard alien types.
	spawnMatrix[BOUNCE_ALIEN][FIRST_WAVE] = Alien::BOUNCE_CUBE;
	spawnMatrix[BOUNCE_ALIEN][SECOND_WAVE] = Alien::BOUNCE_WANDERER;
	spawnMatrix[CHASE_ALIEN][FIRST_WAVE] = Alien::CHASE_RHOMBUS;
	spawnMatrix[CHASE_ALIEN][SECOND_WAVE] = Alien::CHASE_STAR;
	spawnMatrix[ATTACK_ALIEN][FIRST_WAVE] = Alien::ATTACK_TANK;
	spawnMatrix[ATTACK_ALIEN][SECOND_WAVE] = Alien::ATTACK_SHIP;

	// Second and third waves of standard aliens types.
	spawnMatrix[BOUNCE_ALIEN][SECOND_WAVE] = Alien::BOUNCE_WANDERER;
	spawnMatrix[BOUNCE_ALIEN][THIRD_WAVE] = Alien::BOUNCE_STAR;
	spawnMatrix[CHASE_ALIEN][SECOND_WAVE] = Alien::CHASE_STAR;
	spawnMatrix[CHASE_ALIEN][THIRD_WAVE] = Alien::CHASE_SHURIKEN;
	spawnMatrix[ATTACK_ALIEN][SECOND_WAVE] = Alien::ATTACK_SHIP;
	spawnMatrix[ATTACK_ALIEN][THIRD_WAVE] = Alien::ATTACK_ARTILLERY;

	// Third and fourth waves of standard aliens types.
	spawnMatrix[BOUNCE_ALIEN][THIRD_WAVE] = Alien::BOUNCE_STAR;
	spawnMatrix[BOUNCE_ALIEN][LAST_WAVE] = Alien::BOUNCE_HEXAGON;
	spawnMatrix[CHASE_ALIEN][THIRD_WAVE] = Alien::CHASE_SHURIKEN;
	spawnMatrix[CHASE_ALIEN][LAST_WAVE] = Alien::CHASE_CONCAVE;
	spawnMatrix[ATTACK_ALIEN][THIRD_WAVE] = Alien::ATTACK_ARTILLERY;
	spawnMatrix[ATTACK_ALIEN][LAST_WAVE] = Alien::ATTACK_NEUTRON;

	specialAliens[0] = Alien::ROCKET_SHIP;
	specialAliens[1] = Alien::ROCKET_SHIP;
	specialAliens[2] = Alien::ROCKET_SHIP;
	specialAliens[3] = Alien::FLYING_SAUCER;
	specialAliens[4] = Alien::FLYING_SAUCER;
	specialAliens[5] = Alien::PLAYER_CLONE;
	specialAliens[6] = Alien::BLACK_HOLE;
	specialAliens[7] = Alien::MINI_GATE;

	bossAliens[0] = Alien::SNAKE;
	bossAliens[1] = Alien::JELLY;
	bossAliens[2] = Alien::CRUSHER;

	spawnLogic.intervalIncrement = (float) (MAX_STANDARD_ALIEN_SPAWN_INTERVAL - MIN_STANDARD_ALIEN_SPAWN_INTERVAL)
			/ WAVE_INTENSITY_INCREMENT_DURATION;
	spawnLogic.intervalDecrement = (float) (MAX_STANDARD_ALIEN_SPAWN_INTERVAL - MIN_STANDARD_ALIEN_SPAWN_INTERVAL)
			/ WAVE_INTENSITY_DECREMENT_DURATION;

	// This value will increase with each stage (increase the difficulty).
	spawnLogic.midPointDuration = WAVE_INTENSITY_MIDPOINT_INITIAL_DURATION;

	// Initial game stage.
	currentGameStage = SPAWN_STAGE_STARTUP;

	// Initial boss type.
	currentBossType = FIRST_BOSS;

	// Initial waves.
	spawnLogic.currentWave[BOUNCE_ALIEN] = FIRST_WAVE;
	spawnLogic.currentWave[CHASE_ALIEN] = FIRST_WAVE;
	spawnLogic.currentWave[ATTACK_ALIEN] = FIRST_WAVE;

	// Timers.
	stageStartDelay = STAGE_START_DELAY;
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

	if (currentGameStage == SPAWN_STAGE_STARTUP) {
		if (totalElapsedTime > stageStartDelay) {
			// Ready to spawn if all other aliens have been destroyed and the player is active.
			if ((!alienListHead) && (playState->getPlayer()->isActive())) {
				// We have now moved to the spawn stage.
				currentGameStage = SPAWN_STAGE;
				spawnLogic.currentAlienType = BOUNCE_ALIEN;
				spawnLogic.intensity = INCREASING_SPAWN_INTENSITY;
				spawnLogic.currentIntervalStandard = MAX_STANDARD_ALIEN_SPAWN_INTERVAL;
				spawnLogic.currentIntervalSpecial = SPECIAL_ALIEN_SPAWN_INTERVAL;
				spawnLogic.lastSpawnedStandard = 0;
				spawnLogic.lastSpawnedSpecial = 0;

				// Start playing standard music.
				g_worldManager->setMusicType(STANDARD_MUSIC);
				g_worldManager->startMusic();
			}
		}
	}

	if (currentGameStage == SPAWN_STAGE) {
		// See if we need to create more standard aliens.
		if ((totalElapsedTime - spawnLogic.lastSpawnedStandard) >= spawnLogic.currentIntervalStandard) {
			// Ready to spawn another standard alien (if the player is active).
			if (playState->getPlayer()->isActive()) {
				spawnStandardAlien(spawnMatrix[spawnLogic.currentAlienType][spawnLogic.currentWave[spawnLogic.currentAlienType]]);

				// Store the time of the last spawn attempt (may not have actually spawned).
				spawnLogic.lastSpawnedStandard = totalElapsedTime;
			}
		}
		
		// See if we need to create more special aliens.
		if ((totalElapsedTime - spawnLogic.lastSpawnedSpecial) >= spawnLogic.currentIntervalSpecial) {
			// Ready to spawn another special alien (if the player is active).
			if (playState->getPlayer()->isActive()) {
				spawnSpecialAlien(specialAliens[int_rand(0, 7)]);

				// Store the time of the last spawn attempt (may not have actually spawned).
				spawnLogic.lastSpawnedSpecial = totalElapsedTime;
			}
		}

		if (spawnLogic.intensity == INCREASING_SPAWN_INTENSITY) {
			// Build intensity by reducing the spawn frequency.
			spawnLogic.currentIntervalStandard -= (spawnLogic.intervalDecrement * elapsedTime);
			if (spawnLogic.currentIntervalStandard <= MIN_STANDARD_ALIEN_SPAWN_INTERVAL) {
				// Highest intensity reached.
				spawnLogic.intensity = MIDPOINT_SPAWN_INTENSITY;

				// Change the type of standard alien to spawn.
				if (spawnLogic.currentWave[spawnLogic.currentAlienType] < LAST_WAVE) {
					spawnLogic.currentWave[spawnLogic.currentAlienType]++;
				} else {
					spawnLogic.currentWave[spawnLogic.currentAlienType] = FIRST_WAVE;
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
				spawnLogic.intensity = INCREASING_SPAWN_INTENSITY;

				if (spawnLogic.currentAlienType < ATTACK_ALIEN) {
					// Move to the next alien type.
					spawnLogic.currentAlienType++;
				} else {
					// We have reached the last wave of this spawn stage, move to the boss stage.
					currentGameStage = BOSS_STAGE_STARTUP;
					stageStartDelay = (totalElapsedTime + STAGE_START_DELAY);
					g_worldManager->stopMusic();
					g_worldManager->setMusicType(NO_MUSIC);
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

	if (currentGameStage == BOSS_STAGE_STARTUP) {
		if (totalElapsedTime > stageStartDelay) {
			// Ready to spawn if all other aliens have been destroyed and the player is active.
			if ((!alienListHead) && (playState->getPlayer()->isActive())) {
				// We have now moved to the boss stage.
				currentGameStage = BOSS_STAGE;
				g_worldManager->setMusicType(BOSS_MUSIC);
				g_worldManager->startMusic();
				spawnBossAlien(bossAliens[currentBossType]);
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

	if (currentGameStage == BOSS_STAGE) {
		// The boss has been killed, move back to the spawn stage.
		currentGameStage = SPAWN_STAGE_STARTUP;
		stageStartDelay = (totalElapsedTime + STAGE_START_DELAY);
		g_worldManager->stopMusic();

		// Make each stage harder than the one before.
		spawnLogic.midPointDuration += WAVE_INTENSITY_MIDPOINT_DURATION_INCREMENT;

		if (currentBossType < LAST_BOSS) {
			// Move to the next boss type.
			currentBossType++;
		} else {
			// This is the end of the game, start a new game.
			currentBossType = FIRST_BOSS;
			spawnLogic.currentWave[BOUNCE_ALIEN] = FIRST_WAVE;
			spawnLogic.currentWave[CHASE_ALIEN] = FIRST_WAVE;
			spawnLogic.currentWave[ATTACK_ALIEN] = FIRST_WAVE;

			// Make the new game more interesting.
			playState->getNuggetController()->setSpawnMode(ALWAYS_SPAWN_NUGGETS);
		}
	}
}

void AlienController::destroyAliens() {
	if (currentGameStage != BOSS_STAGE) {
		Alien* alien = alienListHead;
		while (alien) {
			this->deactivate(alien);
			playState->getNuggetController()->spawnNugget(alien);

			alien = alien->nextInList;
		}
	}
}

Alien* AlienController::findBestTarget(Missile* missile) {
	static NovaVertex missilePosition;
	static NovaVertex alienPosition;

	Alien* target = NULL;
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

void AlienController::spawnStandardAlien(Alien::AlienType type) {
	switch (type) {
	case Alien::BOUNCE_CUBE:
		spawnBounceCube();
		break;

	case Alien::BOUNCE_WANDERER:
		spawnBounceWanderer();
		break;

	case Alien::BOUNCE_STAR:
		spawnBounceStar();
		break;

	case Alien::BOUNCE_HEXAGON:
		spawnBounceHexagon();
		break;

	case Alien::CHASE_RHOMBUS:
		spawnChaseRhombus();
		break;

	case Alien::CHASE_STAR:
		spawnChaseStar();
		break;

	case Alien::CHASE_SHURIKEN:
		spawnChaseShuriken();
		break;

	case Alien::CHASE_CONCAVE:
		spawnChaseConcave();
		break;

	case Alien::ATTACK_TANK:
		spawnAttackTank();
		break;

	case Alien::ATTACK_SHIP:
		spawnAttackShip();
		break;

	case Alien::ATTACK_ARTILLERY:
		spawnAttackArtillery();
		break;

	default: // ATTACK_NEUTRON
		spawnAttackNeutron();
	}
}

void AlienController::spawnBounceCube() {
	// Find an empty slot.
	for (Alien* alien : bounceCubes) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 3, 6);
			break;
		}
	}
}

void AlienController::spawnBounceWanderer() {
	// Find an empty slot.
	for (Alien* alien : bounceWanderers) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 3, 6);
			break;
		}
	}
}

void AlienController::spawnBounceStar() {
	// Find an empty slot.
	for (Alien* alien : bounceStars) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 3, 6);
			break;
		}
	}
}

void AlienController::spawnBounceHexagon() {
	// Find an empty slot.
	for (Alien* alien : bounceHexagons) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 3, 6);
			break;
		}
	}
}

void AlienController::spawnChaseRhombus() {
	// Find an empty slot.
	for (Alien* alien : chaseRhombi) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnChaseStar() {
	// Find an empty slot.
	for (Alien* alien : chaseStars) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnChaseShuriken() {
	// Find an empty slot.
	for (Alien* alien : chaseShurikens) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnChaseConcave() {
	// Find an empty slot.
	for (Alien* alien : chaseConcaves) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnAttackTank() {
	// Find an empty slot.
	for (Alien* alien : attackTanks) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnAttackShip() {
	// Find an empty slot.
	for (Alien* alien : attackShips) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnAttackArtillery() {
	// Find an empty slot.
	for (Alien* alien : attackArtillery) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnAttackNeutron() {
	// Find an empty slot.
	for (Alien* alien : attackNeutrons) {
		if (!alien->isActive()) {
			// Setup initial position, direction and velocity of the new alien.
			spawnHelper(alien, 2, 4);
			break;
		}
	}
}

void AlienController::spawnSpecialAlien(Alien::AlienType type) {
	switch (type) {
	case Alien::ROCKET_SHIP:
		spawnRocketShip();
		break;

	case Alien::FLYING_SAUCER:
		spawnFlyingSaucer();
		break;

	case Alien::PLAYER_CLONE:
		spawnPlayerClone();
		break;

	case Alien::BLACK_HOLE:
		spawnBlackHole();
		break;

	default: // SMALL_GATE
		spawnMiniGate();
	}
}

void AlienController::spawnRocketShip() {
	if (!rocketShip->isActive()) {
		// Setup initial position, direction and velocity of the new alien.
		float miniumVelocity = 8;
		float maximumVelocity = 16;

		// Find out which spawn zone we should be using (based on the players current location).
		if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
			// Spawn the alien in the right zone.
			rocketShip->moveTo(RIGHT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			rocketShip->setVelocityFromComponents(-float_rand(miniumVelocity, maximumVelocity), 0);

			// Rotate the sprite to match the direction of travel.
			rocketShip->setFacingTowardsDirection(LEFT);
		} else {
			// Spawn the alien in the left zone.
			rocketShip->moveTo(LEFT_SPAWN_POINT_X, float_rand(-60, 60), 0);
			rocketShip->setVelocityFromComponents(float_rand(miniumVelocity, maximumVelocity), 0);

			// Rotate the sprite to match the direction of travel.
			rocketShip->setFacingTowardsDirection(RIGHT);
		}

		// Then add it to the processing loop.
		this->addToList(rocketShip);
	}
}

void AlienController::spawnFlyingSaucer() {
	if (!flyingSaucer->isActive()) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(flyingSaucer, 2, 3);
	}
}

void AlienController::spawnPlayerClone() {
	if (!playerClone->isActive()) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(playerClone, 1, 3);
	}
}

void AlienController::spawnBlackHole() {
	if (!blackHole->isActive()) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(blackHole, 0.1, 0.3);
	}
}

void AlienController::spawnMiniGate() {
	if (!miniGate->isActive()) {
		// Setup initial position, direction and velocity of the new alien.
		spawnHelper(miniGate, 1, 3);
	}
}

void AlienController::spawnBossAlien(Alien::AlienType type) {
	switch (type) {
		case Alien::SNAKE:
			spawnSnake();
			break;

		case Alien::JELLY:
			spawnJelly();
			break;

		default: // CRUSHER
			spawnCrusher();
			break;
	}
}

void AlienController::spawnSnake() {
	// Setup initial position, direction and velocity of the new alien.
	spawnHelper(snake, 6, 9);
}

void AlienController::spawnJelly() {
	// Setup initial position, direction and velocity of the new alien.
	float miniumVelocity = 2;
	float maximumVelocity = 4;
	double horizontalVelocity = 0;
	double verticalVelocity = 0;

	// Find out which spawn zone we should be using (based on the players current location).
	if (playState->getPlayAreaController()->isWithinZone(ZoneIndex::LEFT_SPAWN_ZONE, playState->getPlayer())) {
		// Spawn the alien in the right zone.
		jelly->moveTo(RIGHT_SPAWN_POINT_X, HORIZONTAL_BORDER_POSITION, 0);
		horizontalVelocity = -float_rand(miniumVelocity, maximumVelocity);
		verticalVelocity = -float_rand(miniumVelocity, maximumVelocity);
		jelly->setVelocityFromComponents(-horizontalVelocity, verticalVelocity);
	} else {
		// Spawn the alien in the left zone.
		jelly->moveTo(LEFT_SPAWN_POINT_X, HORIZONTAL_BORDER_POSITION, 0);
		horizontalVelocity = float_rand(miniumVelocity, maximumVelocity);
		verticalVelocity = -float_rand(miniumVelocity, maximumVelocity);
		jelly->setVelocityFromComponents(horizontalVelocity, verticalVelocity);
	}

	// Then add it to the processing loop.
	this->addToList(jelly);
}

void AlienController::spawnCrusher() {
	// Setup initial position, direction and velocity of the new alien.
	crusher->moveTo(0, 0, 0);
	crusher->setVelocityFromComponents(0, 0);

	// Then add it to the processing loop.
	this->addToList(crusher);
}

