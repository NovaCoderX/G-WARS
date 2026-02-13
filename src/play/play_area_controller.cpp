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

#define NUMBER_OF_BORDERS 4
#define NUMBER_OF_BORDER_VERTICES 4

PlayAreaController::PlayAreaController(PlayState* playState) {
	this->playState = playState;
	borderData.staticVertices = NULL;
	grid = NULL;
}

PlayAreaController::~PlayAreaController() {
	if (borderData.staticVertices) {
		delete[] borderData.staticVertices;
		borderData.staticVertices = NULL;
	}

    for (PlayAreaBorder* border : borders) {
        delete border;
    }

    borders.clear();

	if (grid) {
		delete grid;
		grid = NULL;
	}
}

void PlayAreaController::init() {
	for (int i = 0; i < NUMBER_OF_BORDERS; i++) {
		borders.push_back(new PlayAreaBorder(playState));
	}

	// Create the shared vertex row data.
	borderData.staticVertices = new NovaVertex[NUMBER_OF_BORDER_VERTICES];

	// All of the border lines share the same two colors.
	borderData.lowColors[0] = NovaColor(25, 25, 112);
	borderData.lowColors[1] = NovaColor(136, 8, 8);
	borderData.highColors[0] = NovaColor(168, 168, 255);
	borderData.highColors[1] = NovaColor(255, 127, 127);
	borderData.lineColors[0] = borderData.lowColors[0];
	borderData.lineColors[1] = borderData.lowColors[1];

	// Set the shared vertex data for the two vertical borders.
	borderData.staticVertices[0].x = 0;
	borderData.staticVertices[0].y = -HORIZONTAL_BORDER_POSITION;
	borderData.staticVertices[0].z = 0;
	borderData.staticVertices[1].x = 0;
	borderData.staticVertices[1].y = HORIZONTAL_BORDER_POSITION;
	borderData.staticVertices[1].z = 0;

	borders[LEFT_BORDER]->staticVertices[LINE_BEGIN] = &borderData.staticVertices[0];
	borders[LEFT_BORDER]->lineColors[LINE_BEGIN] = &borderData.lineColors[1];
	borders[LEFT_BORDER]->staticVertices[LINE_END] = &borderData.staticVertices[1];
	borders[LEFT_BORDER]->lineColors[LINE_END] = &borderData.lineColors[0];
	borders[LEFT_BORDER]->moveTo(-VERTICAL_BORDER_POSITION, 0, 0);

	borders[RIGHT_BORDER]->staticVertices[LINE_BEGIN] = &borderData.staticVertices[1];
	borders[RIGHT_BORDER]->lineColors[LINE_BEGIN] = &borderData.lineColors[1];
	borders[RIGHT_BORDER]->staticVertices[LINE_END] = &borderData.staticVertices[0];
	borders[RIGHT_BORDER]->lineColors[LINE_END] = &borderData.lineColors[0];
	borders[RIGHT_BORDER]->moveTo(VERTICAL_BORDER_POSITION, 0, 0);

	// Set the shared vertex data for the two horizontal borders.
	borderData.staticVertices[2].x = -VERTICAL_BORDER_POSITION;
	borderData.staticVertices[2].y = 0;
	borderData.staticVertices[2].z = 0;
	borderData.staticVertices[3].x = VERTICAL_BORDER_POSITION;
	borderData.staticVertices[3].y = 0;
	borderData.staticVertices[3].z = 0;

	borders[TOP_BORDER]->staticVertices[LINE_BEGIN] = &borderData.staticVertices[2];
	borders[TOP_BORDER]->lineColors[LINE_BEGIN] = &borderData.lineColors[0];
	borders[TOP_BORDER]->staticVertices[LINE_END] = &borderData.staticVertices[3];
	borders[TOP_BORDER]->lineColors[LINE_END] = &borderData.lineColors[1];
	borders[TOP_BORDER]->moveTo(0, HORIZONTAL_BORDER_POSITION, 0);

	borders[BOTTOM_BORDER]->staticVertices[LINE_BEGIN] = &borderData.staticVertices[3];
	borders[BOTTOM_BORDER]->lineColors[LINE_BEGIN] = &borderData.lineColors[0];
	borders[BOTTOM_BORDER]->staticVertices[LINE_END] = &borderData.staticVertices[2];
	borders[BOTTOM_BORDER]->lineColors[LINE_END] = &borderData.lineColors[1];
	borders[BOTTOM_BORDER]->moveTo(0, -HORIZONTAL_BORDER_POSITION, 0);

	// Create the grid if enabled.
	Yamlish* yamlish = g_worldManager->getYamlish();
	bool enabled = yamlish->getBool("playfield.enabled", true);
	if (enabled) {
		grid = new PlayAreaGrid(playState);
		grid->init();
	}
}

void PlayAreaController::syncOptions() {
	Yamlish* yamlish = g_worldManager->getYamlish();
	bool enabled = yamlish->getBool("playfield.enabled", true);
	if (enabled) {
		logMessage("Playfield grid enabled\n");
		if (!grid) {
			grid = new PlayAreaGrid(playState);
			grid->init();
		}
	} else {
		logMessage("Playfield grid disabled\n");
		if (grid) {
			delete grid;
			grid = NULL;
		}
	}
}

void PlayAreaController::smartBombNotification() {
	if (grid) {
		grid->smartBombNotification();
	}
}

void PlayAreaController::update(float elapsedTime) {
	static NovaVertex locationCCS;
	bool anyVisible = false;

	// Determine which borders are visible this frame.
	locationCCS = (borders[LEFT_BORDER]->getPositionWCS() * playState->getCamera()->getWorldToCameraMatrix());
	if (horizontalProject(locationCCS.x, locationCCS.z) >= 0) {
		anyVisible = true;
		borders[LEFT_BORDER]->setVisible(true);
	} else {
		borders[LEFT_BORDER]->setVisible(false);
	}

	locationCCS = (borders[RIGHT_BORDER]->getPositionWCS() * playState->getCamera()->getWorldToCameraMatrix());
	if (horizontalProject(locationCCS.x, locationCCS.z) < DISPLAY_WITDH) {
		anyVisible = true;
		borders[RIGHT_BORDER]->setVisible(true);
	} else {
		borders[RIGHT_BORDER]->setVisible(false);
	}

	locationCCS = (borders[TOP_BORDER]->getPositionWCS() * playState->getCamera()->getWorldToCameraMatrix());
	if (verticalProject(locationCCS.y, locationCCS.z) < DISPLAY_HEIGHT) {
		anyVisible = true;
		borders[TOP_BORDER]->setVisible(true);
	} else {
		borders[TOP_BORDER]->setVisible(false);
	}

	locationCCS = (borders[BOTTOM_BORDER]->getPositionWCS() * playState->getCamera()->getWorldToCameraMatrix());
	if (verticalProject(locationCCS.y, locationCCS.z) >= 0) {
		anyVisible = true;
		borders[BOTTOM_BORDER]->setVisible(true);
	} else {
		borders[BOTTOM_BORDER]->setVisible(false);
	}

	// If any border is visible, we must color cycle all of them to keep in sync.
	if (anyVisible) {
		for (int i = 0; i < 2; i++) {
			// Make time updates more granular.
			if (borderData.lineColors[i].brighten((elapsedTime / 10), borderData.highColors[i])) {
				// Reset.
				borderData.lineColors[i] = borderData.lowColors[i];
			}
		}
	}

	if (grid) {
		grid->update(elapsedTime);
	}
}

void PlayAreaController::draw() {
	// Grid must be drawn first (overlap).
	if (grid) {
		grid->draw();
	}

	for (int i = 0; i < NUMBER_OF_BORDERS; i++) {
		if (borders[i]->isVisible()) {
			borders[i]->draw();
		}
	}
}

bool PlayAreaController::isWithinPlayArea(Sprite *object) {
	// Get object's WCS position.
	NovaVertex objectWCS = object->getPositionWCS();

	if ((objectWCS.x - object->getBoundingSphere()) <= borders[LEFT_BORDER]->getPositionWCS().x) {
		return false;
	}

	if ((objectWCS.x + object->getBoundingSphere()) >= borders[RIGHT_BORDER]->getPositionWCS().x) {
		return false;
	}

	if ((objectWCS.y + object->getBoundingSphere()) >= borders[TOP_BORDER]->getPositionWCS().y) {
		return false;
	}

	if ((objectWCS.y - object->getBoundingSphere()) <= borders[BOTTOM_BORDER]->getPositionWCS().y) {
		return false;
	}

	// Object is within the play area.
	return true;
}

bool PlayAreaController::isWithinPlayArea(BorderIndex borderIndex, Sprite *object) {
	NovaVertex objectWCS = object->getPositionWCS();

	// Object is within the play area until proved otherwise...
	bool result = true;

	switch (borderIndex) {
	case LEFT_BORDER:
		if ((objectWCS.x - object->getBoundingSphere()) <= borders[LEFT_BORDER]->getPositionWCS().x) {
			return false;
		}
		break;

	case RIGHT_BORDER:
		if ((objectWCS.x + object->getBoundingSphere()) >= borders[RIGHT_BORDER]->getPositionWCS().x) {
			return false;
		}
		break;

	case TOP_BORDER:
		if ((objectWCS.y + object->getBoundingSphere()) >= borders[TOP_BORDER]->getPositionWCS().y) {
			result = false;
		}
		break;

	case BOTTOM_BORDER:
		if ((objectWCS.y - object->getBoundingSphere()) <= borders[BOTTOM_BORDER]->getPositionWCS().y) {
			result = false;
		}
		break;
	}

	// Object is within the vertical play area.
	return result;
}

bool PlayAreaController::isWithinZone(ZoneIndex zoneIndex, Sprite *object) {
	// Object is not within the zone until proved otherwise...
	bool result = false;

	// Get object's WCS position.
	NovaVertex objectWCS = object->getPositionWCS();

	if (zoneIndex == LEFT_SPAWN_ZONE) {
		if (objectWCS.x < (borders[LEFT_BORDER]->getPositionWCS().x + SPAWN_ZONE_SIZE)) {
			result = true;
		}
	} else {
		if (objectWCS.x > (borders[RIGHT_BORDER]->getPositionWCS().x - SPAWN_ZONE_SIZE)) {
			result = true;
		}
	}

	return result;
}

