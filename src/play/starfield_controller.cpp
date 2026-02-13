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

#define DEFULT_NUM_STARS 0
#define DEFAULT_STARFIELD_DEPTH 100

StarfieldController::StarfieldController(PlayState* playState) {
	this->playState = playState;
}

StarfieldController::~StarfieldController() {
	for (Star* star : stars) {
		delete star;
	}

	stars.clear();
}

void StarfieldController::init() {
	Yamlish* yamlish = g_worldManager->getYamlish();

	// Create the starfield if enabled.
	bool enabled = yamlish->getBool("starfield.enabled", true);
	if (enabled) {
		float depth, horizontalExtents, verticalExtents;
		int red, green, blue;

		int numStarsLayer1 = yamlish->getInt("starfield.background.density", DEFULT_NUM_STARS);
		int numStarsLayer2 = yamlish->getInt("starfield.middleground.density", DEFULT_NUM_STARS);
		int numStarsLayer3 = yamlish->getInt("starfield.foreground.density", DEFULT_NUM_STARS);

		int numStars = (numStarsLayer1 + numStarsLayer2 + numStarsLayer3);
		for (int i = 0; i < numStars; i++) {
			stars.push_back(new Star(playState));
		}

		if (numStarsLayer1 > 0) {
			depth = yamlish->getInt("starfield.background.depth", DEFAULT_STARFIELD_DEPTH);

			std::vector<float> extents = yamlish->getFloatList("starfield.background.extents");
			horizontalExtents = extents[0];
			verticalExtents = extents[1];

			std::vector<int> colors = yamlish->getIntList("starfield.background.colors");
			red = colors[0];
			green = colors[1];
			blue = colors[2];
			for (int i = 0; i < numStarsLayer1; i++) {
				stars[i]->init(NovaColor(red, green, blue), depth, horizontalExtents, verticalExtents);
			}
		}

		if (numStarsLayer2 > 0) {
			depth = yamlish->getInt("starfield.middleground.depth", DEFAULT_STARFIELD_DEPTH);

			std::vector<float> extents = yamlish->getFloatList("starfield.middleground.extents");
			horizontalExtents = extents[0];
			verticalExtents = extents[1];

			std::vector<int> colors = yamlish->getIntList("starfield.middleground.colors");
			red = colors[0];
			green = colors[1];
			blue = colors[2];
			for (int i = numStarsLayer1; i < (numStarsLayer1 + numStarsLayer2); i++) {
				stars[i]->init(NovaColor(red, green, blue), depth, horizontalExtents, verticalExtents);
			}
		}

		if (numStarsLayer3 > 0) {
			depth = yamlish->getInt("starfield.foreground.depth", DEFAULT_STARFIELD_DEPTH);

			std::vector<float> extents = yamlish->getFloatList("starfield.foreground.extents");
			horizontalExtents = extents[0];
			verticalExtents = extents[1];

			std::vector<int> colors = yamlish->getIntList("starfield.foreground.colors");
			red = colors[0];
			green = colors[1];
			blue = colors[2];
			for (int i = (numStarsLayer1 + numStarsLayer2); i < numStars; i++) {
				stars[i]->init(NovaColor(red, green, blue), depth, horizontalExtents, verticalExtents);
			}
		}
	}
}

void StarfieldController::syncOptions() {
	Yamlish* yamlish = g_worldManager->getYamlish();
	bool enabled = yamlish->getBool("starfield.enabled", true);
	if (enabled) {
		logMessage("Starfield enabled\n");
		if (stars.empty()) {
			this->init();
		}
	} else {
		logMessage("Starfield disabled\n");
		if (!stars.empty()) {
			for (Star* star : stars) {
				delete star;
			}

			stars.clear();
		}
	}
}

void StarfieldController::update(float elapsedTime) {
	for (Star* star : stars) {
		star->update(elapsedTime);
	}
}

void StarfieldController::draw() {
	for (Star* star : stars) {
		if (star->isVisible()) {
			star->draw();
		}
	}
}

