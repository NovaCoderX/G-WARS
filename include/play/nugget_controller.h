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

class NuggetController {
public:
	NuggetController(PlayState* playState);
	~NuggetController();

	void init();
	void update(float elapsedTime);
	void checkCollisions();
	void draw();
	void spawnNugget(NuggetType nuggetType, const NovaVertex &location);
	void deactivate(Nugget* nugget);

private:
	PlayState* playState;

	// Maintains a linked list of nuggets in the world.
	Nugget* nuggetListHead;

	std::vector<MultiplierNugget*> multiplierNuggets;
	std::vector<PowerUpNugget*> powerUpNuggets;
	std::vector<ExtraLifeNugget*> extraLifeNuggets;
	std::vector<ExtraBombNugget*> extraBombNuggets;

	void addToList(Nugget* nugget);
	void removeFromList(Nugget* nugget);
	void spawnMultiplierNugget(const NovaVertex &location);
	void spawnPowerUpNugget(const NovaVertex &location);
	void spawnExtraLifeNugget(const NovaVertex &location);
	void spawnExtraBombNugget(const NovaVertex &location);
};

#endif // __NUGGET_CONTROLLER_H
