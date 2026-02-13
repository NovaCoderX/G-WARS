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

#define NUMBER_OF_MULTIPLIER_NUGGETS 18
#define NUMBER_OF_POWER_UP_NUGGETS 3
#define NUMBER_OF_EXTRA_LIFE_NUGGETS 3
#define NUMBER_OF_EXTRA_BOMB_NUGGETS 3

NuggetController::NuggetController(PlayState* playState) {
	this->playState = playState;
	nuggetListHead = NULL;
}

NuggetController::~NuggetController() {
	for (Nugget* nugget : multiplierNuggets) {
		delete nugget;
	}

	multiplierNuggets.clear();

	for (Nugget* nugget : powerUpNuggets) {
		delete nugget;
	}

	powerUpNuggets.clear();

	for (Nugget* nugget : extraLifeNuggets) {
		delete nugget;
	}

	extraLifeNuggets.clear();

	for (Nugget* nugget : extraBombNuggets) {
		delete nugget;
	}

	extraBombNuggets.clear();
}

void NuggetController::init() {
	for (int i = 0; i < NUMBER_OF_MULTIPLIER_NUGGETS; i++) {
		multiplierNuggets.push_back(new MultiplierNugget(playState));
	}

	for (int i = 0; i < NUMBER_OF_POWER_UP_NUGGETS; i++) {
		powerUpNuggets.push_back(new PowerUpNugget(playState));
	}

	for (int i = 0; i < NUMBER_OF_EXTRA_LIFE_NUGGETS; i++) {
		extraLifeNuggets.push_back(new ExtraLifeNugget(playState));
	}

	for (int i = 0; i < NUMBER_OF_EXTRA_BOMB_NUGGETS; i++) {
		extraBombNuggets.push_back(new ExtraBombNugget(playState));
	}
}

void NuggetController::update(float elapsedTime) {
	// Update any active nuggets.
	Nugget* nugget = nuggetListHead;
	while (nugget) {
		nugget->update(elapsedTime);

		nugget = nugget->nextInList;
	}
}

void NuggetController::checkCollisions() {
	Player* player = playState->getPlayer();

	// Check for collisions.
	Nugget* nugget = nuggetListHead;
	while (nugget && player->isActive()) {
		if (nugget->isVisible()) {
			// See if this nugget has just hit the player.
			nugget->checkCollision(player);
		}

		nugget = nugget->nextInList;
	}
}

void NuggetController::draw() {
	// Draw the visible objects.
	Nugget* nugget = nuggetListHead;
	while (nugget) {
		if (nugget->isVisible()) {
			nugget->draw();
		}

		nugget = nugget->nextInList;
	}
}

void NuggetController::spawnNugget(NuggetType nuggetType, const NovaVertex &location) {
	switch (nuggetType) {
	case MULTIPLIER_NUGGET:
		this->spawnMultiplierNugget(location);
		break;

	case POWER_UP_NUGGET:
		this->spawnPowerUpNugget(location);
		break;

	case EXTRA_LIFE_NUGGET:
		this->spawnExtraLifeNugget(location);
		break;

	default: // EXTRA_BOMB_NUGGET
		this->spawnExtraBombNugget(location);
	}
}

void NuggetController::deactivate(Nugget* nugget) {
	this->removeFromList(nugget);
}

void NuggetController::addToList(Nugget* nugget) {
	/*
	 ** If no objects have been inserted into the world yet
	 ** then make this object the head of the object list.
	 */
	if (!nuggetListHead) {
		nuggetListHead = nugget;
		nuggetListHead->priorInList = NULL;
		nuggetListHead->nextInList = NULL;
	} else {
		/*
		 ** Otherwise find the last object in the world and then
		 ** set the passed object as the new last object in the
		 ** world.
		 */
		Nugget* last_in_list = nuggetListHead;

		/*
		 ** Find the last object in the world
		 */
		while (last_in_list->nextInList) {
			/*
			 ** See if the passed object has already been inserted
			 */
			if (last_in_list == nugget)
				return;

			last_in_list = last_in_list->nextInList;
		}

		/*
		 ** Adjust pointers
		 */
		last_in_list->nextInList = nugget;
		nugget->priorInList = last_in_list;
		nugget->nextInList = NULL;
	}

	// Object is now part of the world (will be drawn).
	nugget->setActive(true);
}

void NuggetController::removeFromList(Nugget* nugget) {
	Nugget* previousObj = nugget->priorInList;
	Nugget* nextObj = nugget->nextInList;

	// Have to treat list head removal differently.
	if (nugget == nuggetListHead) {
		nuggetListHead = nextObj;	// Assign new list head.
		if (nuggetListHead)
			nuggetListHead->priorInList = NULL;
	} else {
		if (previousObj)
			previousObj->nextInList = nextObj;

		// Set prior reference of the next object if there is one.
		if (nextObj)
			nextObj->priorInList = previousObj;
	}

	// Object is no longer part of the world (will not be drawn).
	nugget->setActive(false);
}

void NuggetController::spawnMultiplierNugget(const NovaVertex &location) {
	MultiplierNugget* nugget = NULL;

	// Find an empty slot.
	for (uint i = 0; i < multiplierNuggets.size(); i++) {
		if (!multiplierNuggets[i]->isActive()) {
			nugget = multiplierNuggets[i];
			break;
		}
	}

	if (nugget) {
		nugget->moveTo(location);

		// Then add it to the processing loop.
		this->addToList(nugget);
	}
}

void NuggetController::spawnPowerUpNugget(const NovaVertex &location) {
	PowerUpNugget* nugget = NULL;

	// Find an empty slot.
	for (uint i = 0; i < powerUpNuggets.size(); i++) {
		if (!powerUpNuggets[i]->isActive()) {
			nugget = powerUpNuggets[i];
			break;
		}
	}

	if (nugget) {
		nugget->moveTo(location);

		// Then add it to the processing loop.
		this->addToList(nugget);
	}
}

void NuggetController::spawnExtraLifeNugget(const NovaVertex &location) {
	ExtraLifeNugget* nugget = NULL;

	// Find an empty slot.
	for (uint i = 0; i < extraLifeNuggets.size(); i++) {
		if (!extraLifeNuggets[i]->isActive()) {
			nugget = extraLifeNuggets[i];
			break;
		}
	}

	if (nugget) {
		nugget->moveTo(location);

		// Then add it to the processing loop.
		this->addToList(nugget);
	}
}

void NuggetController::spawnExtraBombNugget(const NovaVertex &location) {
	ExtraBombNugget* nugget = NULL;

	// Find an empty slot.
	for (uint i = 0; i < extraBombNuggets.size(); i++) {
		if (!extraBombNuggets[i]->isActive()) {
			nugget = extraBombNuggets[i];
			break;
		}
	}

	if (nugget) {
		nugget->moveTo(location);

		// Then add it to the processing loop.
		this->addToList(nugget);
	}
}




