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
#ifndef __ALIEN_CONTROLLER_H
#define __ALIEN_CONTROLLER_H

class AlienController {
	friend class MissileController;
public:
	AlienController(PlayState* playState);
	~AlienController();

	void init();
	void update(float elapsedTime);
	void checkCollisions();
	void draw();
	void deactivate(Alien* alien);
	void destroyAllLife();
	Alien* findBestTarget(Missile* missile);
    void applyGravitionalPull(Player* player);
    bool isWithinBlackHole(const NovaVertex &positionCCS);

private:
	PlayState* playState;

	// Maintains a linked list of aliens in the world.
	Alien* alienListHead;

	// Standard aliens.
	std::vector<BounceCube*> bounceCubes;
	std::vector<BounceWanderer*> bounceWanderers;
	std::vector<BounceStar*> bounceStars;
	std::vector<BounceHexagon*> bounceHexagons;
	std::vector<ChaseRhombus*> chaseRhombi;
	std::vector<ChaseStar*> chaseStars;
	std::vector<ChaseShuriken*> chaseShurikens;
	std::vector<ChaseConcave*> chaseConcaves;
	std::vector<AttackTank*> attackTanks;
	std::vector<AttackShip*> attackShips;
	std::vector<AttackArtillery*> attackArtillery;
	std::vector<AttackNeutron*> attackNeutrons;

	// Special aliens.
	RocketShip* rocketShip;
	FlyingSaucer* flyingSaucer;
	PlayerClone* playerClone;
	BlackHole* blackHole;
	MiniGate* miniGate;

	// Boss aliens.
	Snake *snake;

	struct SpawnLogic {
		int currentGameStage;
		int currentWave;
		int currentWaveType[3];
		int intensity;
		float currentIntervalStandard;
		float currentIntervalSpecial;
		float intervalIncrement;
		float intervalDecrement;
		float midPointStartTime;
		float midPointDuration;
		float lastSpawnedStandard;
		float lastSpawnedSpecial;
		float bossStageStartTime;
	} spawnLogic;

	float totalElapsedTime;

	void addToList(Alien* alien);
	void removeFromList(Alien* alien);
	void spawnHelper(Alien* alien, float miniumVelocity, float maximumVelocity);
	void spawnStandardAlien(AlienType type);
	void spawnBounceCube();
	void spawnBounceWanderer();
	void spawnBounceStar();
	void spawnBounceHexagon();
	void spawnChaseRhombus();
	void spawnChaseStar();
	void spawnChaseShuriken();
	void spawnChaseConcave();
	void spawnAttackTank();
	void spawnAttackShip();
	void spawnAttackArtillery();
	void spawnAttackNeutron();
	void spawnSpecialAlien(AlienType type);
	void spawnRocketShip();
	void spawnFlyingSaucer();
	void spawnPlayerClone();
	void spawnBlackHole();
	void spawnMiniGate();
	void spawnBossAlien();
};

#endif // __ALIEN_CONTROLLER_H
