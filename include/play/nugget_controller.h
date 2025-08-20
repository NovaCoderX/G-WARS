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
#ifndef __NUGGET_CONTROLLER_H
#define __NUGGET_CONTROLLER_H

enum NuggetSpawnMode {
	NEVER_SPAWN_NUGGETS = 0,
	RANDOMLY_SPAWN_NUGGETS = 1,
	ALWAYS_SPAWN_NUGGETS = 2
};

class NuggetController {
public:
	NuggetController(PlayState* playState);
	~NuggetController();

	void init();
	void update(float elapsedTime);
	void checkCollisions();
	void draw();
	void spawnNugget(Alien* alien);
	void deactivate(Nugget* nugget);

	NuggetSpawnMode getSpawnMode() const {
		return spawnMode;
	}

	NuggetSpawnMode setSpawnMode(NuggetSpawnMode spawnMode) {
		NuggetSpawnMode perviousMode = this->spawnMode;
		this->spawnMode = spawnMode;
		return perviousMode;
	}

private:
	PlayState* playState;

	// Maintains a linked list of nuggets in the world.
	Nugget* nuggetListHead;

	std::vector<MultiplierNugget*> multiplierNuggets;
	std::vector<PowerUpNugget*> powerUpNuggets;
	NuggetSpawnMode spawnMode;

	void addToList(Nugget* alien);
	void removeFromList(Nugget* alien);
	void spawnMultiplierNugget(Alien* alien);
	void spawnPowerUpNugget(Alien* alien);
};

#endif // __NUGGET_CONTROLLER_H
