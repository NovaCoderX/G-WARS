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
#ifndef __PLAY_AREA_CONTROLLER_H
#define __PLAY_AREA_CONTROLLER_H

#define VERTICAL_BORDER_POSITION 100
#define HORIZONTAL_BORDER_POSITION 80
#define LEFT_SPAWN_POINT_X -90
#define LEFT_SPAWN_POINT_Y 0
#define RIGHT_SPAWN_POINT_X 90
#define RIGHT_SPAWN_POINT_Y 0
#define TOP_SPAWN_POINT_Y 70
#define BOTTOM_SPAWN_POINT_Y -70
#define SPAWN_ZONE_SIZE 90

enum BorderIndex {
	LEFT_BORDER = 0, RIGHT_BORDER = 1, TOP_BORDER = 2, BOTTOM_BORDER = 3
};

enum ZoneIndex {
	LEFT_SPAWN_ZONE = 0, RIGHT_SPAWN_ZONE = 1
};

class PlayAreaController {
public:
	PlayAreaController(PlayState* playState);
	~PlayAreaController();

	NovaVertex getBorderPositionWCS(BorderIndex borderIndex) const {
		return borders[borderIndex]->getPositionWCS();
	}

	bool isVisible(BorderIndex borderIndex) {
		return borders[borderIndex]->isVisible();
	}

	bool isWithinPlayArea(Sprite *object);
	bool isWithinPlayArea(BorderIndex borderIndex, Sprite *object);
	bool isWithinZone(ZoneIndex zoneIndex, Sprite *object);
	void init();
	void syncOptions();
	void smartBombNotification();
	void update(float elapsedTime);
	void draw();

private:
	PlayState* playState;

	struct BorderData {
		NovaVertex *staticVertices;
		NovaColor lineColors[2];
		NovaColor highColors[2];
		NovaColor lowColors[2];
	} borderData;

	std::vector<PlayAreaBorder*> borders;
	PlayAreaGrid* grid;
};

#endif // __PLAY_AREA_CONTROLLER_H
