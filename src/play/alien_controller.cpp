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

static AlienType spawnMatrix[3][4];
static AlienType specialAliens[10];
static AlienType bossAliens[3];

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
	miniCrusher = NULL;

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

	if (miniCrusher) {
		delete miniCrusher;
		miniCrusher = NULL;
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
	miniCrusher = new MiniCrusher(playState);

	// Boss aliens.
	snake = new Snake(playState);
	jelly = new Jelly(playState);
	crusher = new Crusher(playState);

	// First and second waves of standard alien types.
	spawnMatrix[BOUNCE_ALIEN][FIRST_WAVE] = BOUNCE_CUBE_ALIEN;
	spawnMatrix[BOUNCE_ALIEN][SECOND_WAVE] = BOUNCE_WANDERER_ALIEN;
	spawnMatrix[CHASE_ALIEN][FIRST_WAVE] = CHASE_RHOMBUS_ALIEN;
	spawnMatrix[CHASE_ALIEN][SECOND_WAVE] = CHASE_STAR_ALIEN;
	spawnMatrix[ATTACK_ALIEN][FIRST_WAVE] = ATTACK_TANK_ALIEN;
	spawnMatrix[ATTACK_ALIEN][SECOND_WAVE] = ATTACK_SHIP_ALIEN;

	// Second and third waves of standard aliens types.
	spawnMatrix[BOUNCE_ALIEN][SECOND_WAVE] = BOUNCE_WANDERER_ALIEN;
	spawnMatrix[BOUNCE_ALIEN][THIRD_WAVE] = BOUNCE_STAR_ALIEN;
	spawnMatrix[CHASE_ALIEN][SECOND_WAVE] = CHASE_STAR_ALIEN;
	spawnMatrix[CHASE_ALIEN][THIRD_WAVE] = CHASE_SHURIKEN_ALIEN;
	spawnMatrix[ATTACK_ALIEN][SECOND_WAVE] = ATTACK_SHIP_ALIEN;
	spawnMatrix[ATTACK_ALIEN][THIRD_WAVE] = ATTACK_ARTILLERY_ALIEN;

	// Third and fourth waves of standard aliens types.
	spawnMatrix[BOUNCE_ALIEN][THIRD_WAVE] = BOUNCE_STAR_ALIEN;
	spawnMatrix[BOUNCE_ALIEN][LAST_WAVE] = BOUNCE_HEXAGON_ALIEN;
	spawnMatrix[CHASE_ALIEN][THIRD_WAVE] = CHASE_SHURIKEN_ALIEN;
	spawnMatrix[CHASE_ALIEN][LAST_WAVE] = CHASE_CONCAVE_ALIEN;
	spawnMatrix[ATTACK_ALIEN][THIRD_WAVE] = ATTACK_ARTILLERY_ALIEN;
	spawnMatrix[ATTACK_ALIEN][LAST_WAVE] = ATTACK_NEUTRON_ALIEN;

	specialAliens[0] = ROCKET_SHIP_ALIEN;
	specialAliens[1] = ROCKET_SHIP_ALIEN;
	specialAliens[2] = ROCKET_SHIP_ALIEN;
	specialAliens[3] = FLYING_SAUCER_ALIEN;
	specialAliens[4] = FLYING_SAUCER_ALIEN;
	specialAliens[5] = PLAYER_CLONE_ALIEN;
	specialAliens[6] = BLACK_HOLE_ALIEN;
	specialAliens[7] = MINI_GATE_ALIEN;
	specialAliens[8] = MINI_CRUSHER_ALIEN;
	specialAliens[9] = MINI_CRUSHER_ALIEN;

	bossAliens[0] = SNAKE_ALIEN ;
	bossAliens[1] = JELLY_ALIEN ;
	bossAliens[2] = CRUSHER_ALIEN ;

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
#ifdef TIME_DEMO_ENABLED
		stageStartDelay = 0;
#else
		stageStartDelay = STAGE_START_DELAY;
#endif

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
			// Ready to spawn when the player is active.
			if (playState->getPlayer()->isActive()) {
				// We have now moved to the spawn stage.
				currentGameStage = SPAWN_STAGE;
				spawnLogic.currentAlienType = BOUNCE_ALIEN;
				spawnLogic.intensity = INCREASING_SPAWN_INTENSITY;
				spawnLogic.currentIntervalStandard = MAX_STANDARD_ALIEN_SPAWN_INTERVAL;
				spawnLogic.currentIntervalSpecial = SPECIAL_ALIEN_SPAWN_INTERVAL;
				spawnLogic.lastSpawnedStandard = totalElapsedTime;
				spawnLogic.lastSpawnedSpecial = totalElapsedTime;

				// Start playing music.
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
		
#ifndef TIME_DEMO_ENABLED
		// See if we need to create more special aliens.
		if ((totalElapsedTime - spawnLogic.lastSpawnedSpecial) >= spawnLogic.currentIntervalSpecial) {
			// Ready to spawn another special alien (if the player is active).
			if (playState->getPlayer()->isActive()) {
				spawnSpecialAlien(specialAliens[int_rand(0, 9)]);

				// Store the time of the last spawn attempt (may not have actually spawned).
				spawnLogic.lastSpawnedSpecial = totalElapsedTime;
			}
		}
#endif

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

	if (alien->getAlienCategory() == Alien::BOSS_ALIEN) {
		// A boss has been killed, move back to the spawn stage.
		currentGameStage = SPAWN_STAGE_STARTUP;
		stageStartDelay = (totalElapsedTime + STAGE_START_DELAY);
		g_worldManager->stopMusic();
		g_worldManager->setMusicType(GAMEPLAY_MUSIC);

		// Make each spawn stage harder than the one before.
		spawnLogic.midPointDuration += WAVE_INTENSITY_MIDPOINT_DURATION_INCREMENT;

		if (currentBossType < LAST_BOSS) {
			// Move to the next boss type.
			currentBossType++;
		} else {
			// This is the end of the game, display a message to the user.
			if (playState->getPlayer()->getNumLives()) {
				// Only display this message if the player is still alive.
				playState->getHudController()->getMissionCompletePanel()->setVisible(true);
			}

			// start a new (harder) game.
			currentBossType = FIRST_BOSS;
			spawnLogic.currentWave[BOUNCE_ALIEN] = FIRST_WAVE;
			spawnLogic.currentWave[CHASE_ALIEN] = FIRST_WAVE;
			spawnLogic.currentWave[ATTACK_ALIEN] = FIRST_WAVE;
		}
	}
}

void AlienController::smartBombNotification() {
	Alien* alien = alienListHead;
	while (alien) {
		alien->smartBombNotification();

		alien = alien->nextInList;
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
		bool candidate = false;

		// Only consider standard aliens as targets.
		if (alien->getAlienCategory() == Alien::STANDARD_ALIEN) {
			// Only consider targets that the missile is heading towards.
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

void AlienController::spawnStandardAlien(AlienType type) {
	switch (type) {
	case BOUNCE_CUBE_ALIEN:
		spawnBounceCube();
		break;

	case BOUNCE_WANDERER_ALIEN:
		spawnBounceWanderer();
		break;

	case BOUNCE_STAR_ALIEN:
		spawnBounceStar();
		break;

	case BOUNCE_HEXAGON_ALIEN:
		spawnBounceHexagon();
		break;

	case CHASE_RHOMBUS_ALIEN:
		spawnChaseRhombus();
		break;

	case CHASE_STAR_ALIEN:
		spawnChaseStar();
		break;

	case CHASE_SHURIKEN_ALIEN:
		spawnChaseShuriken();
		break;

	case CHASE_CONCAVE_ALIEN:
		spawnChaseConcave();
		break;

	case ATTACK_TANK_ALIEN:
		spawnAttackTank();
		break;

	case ATTACK_SHIP_ALIEN:
		spawnAttackShip();
		break;

	case ATTACK_ARTILLERY_ALIEN:
		spawnAttackArtillery();
		break;

	default: // ATTACK_NEUTRON_ALIEN
		spawnAttackNeutron();
	}
}

void AlienController::spawnBounceCube() {
	// Find an empty slot.
	for (Alien* alien : bounceCubes) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnBounceWanderer() {
	// Find an empty slot.
	for (Alien* alien : bounceWanderers) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnBounceStar() {
	// Find an empty slot.
	for (Alien* alien : bounceStars) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnBounceHexagon() {
	// Find an empty slot.
	for (Alien* alien : bounceHexagons) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnChaseRhombus() {
	// Find an empty slot.
	for (Alien* alien : chaseRhombi) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnChaseStar() {
	// Find an empty slot.
	for (Alien* alien : chaseStars) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnChaseShuriken() {
	// Find an empty slot.
	for (Alien* alien : chaseShurikens) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnChaseConcave() {
	// Find an empty slot.
	for (Alien* alien : chaseConcaves) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnAttackTank() {
	// Find an empty slot.
	for (Alien* alien : attackTanks) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnAttackShip() {
	// Find an empty slot.
	for (Alien* alien : attackShips) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnAttackArtillery() {
	// Find an empty slot.
	for (Alien* alien : attackArtillery) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnAttackNeutron() {
	// Find an empty slot.
	for (Alien* alien : attackNeutrons) {
		if (!alien->isActive()) {
			// Add it to the processing loop.
			this->addToList(alien);
			break;
		}
	}
}

void AlienController::spawnSpecialAlien(AlienType type) {
	switch (type) {
	case ROCKET_SHIP_ALIEN:
		spawnRocketShip();
		break;

	case FLYING_SAUCER_ALIEN:
		spawnFlyingSaucer();
		break;

	case PLAYER_CLONE_ALIEN:
		spawnPlayerClone();
		break;

	case BLACK_HOLE_ALIEN:
		spawnBlackHole();
		break;

	case MINI_GATE_ALIEN:
		spawnMiniGate();
		break;

	default: // MINI_CRUSHER_ALIEN
		spawnMiniCrusher();
	}
}

void AlienController::spawnRocketShip() {
	if (!rocketShip->isActive()) {
		// Add it to the processing loop.
		this->addToList(rocketShip);
	}
}

void AlienController::spawnFlyingSaucer() {
	if (!flyingSaucer->isActive()) {
		// Add it to the processing loop.
		this->addToList(flyingSaucer);
	}
}

void AlienController::spawnPlayerClone() {
	if (!playerClone->isActive()) {
		// Add it to the processing loop.
		this->addToList(playerClone);
	}
}

void AlienController::spawnBlackHole() {
	if (!blackHole->isActive()) {
		// Add it to the processing loop.
		this->addToList(blackHole);
	}
}

void AlienController::spawnMiniGate() {
	if (!miniGate->isActive()) {
		// Add it to the processing loop.
		this->addToList(miniGate);
	}
}

void AlienController::spawnMiniCrusher() {
	if (!miniCrusher->isActive()) {
		// Add it to the processing loop.
		this->addToList(miniCrusher);
	}
}

void AlienController::spawnBossAlien(AlienType type) {
	switch (type) {
		case SNAKE_ALIEN :
			spawnSnake();
			break;

		case JELLY_ALIEN :
			spawnJelly();
			break;

		default: // CRUSHER_ALIEN
			spawnCrusher();
			break;
	}
}

void AlienController::spawnSnake() {
	if (!snake->isActive()) {
		// Add it to the processing loop.
		this->addToList(snake);
	}
}

void AlienController::spawnJelly() {
	if (!jelly->isActive()) {
		// Add it to the processing loop.
		this->addToList(jelly);
	}
}

void AlienController::spawnCrusher() {
	if (!crusher->isActive()) {
		// Add it to the processing loop.
		this->addToList(crusher);
	}
}

