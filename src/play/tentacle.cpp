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

#define NUMBER_OF_SEGMENTS 12
#define MAX_SEGMENT_SIZE 6
#define MIN_SEGMENT_SIZE 2
#define TENTACLE_LENGTH 40.0
#define MOVEMENT_AMPLITUDE 10.0
#define MOVEMENT_FREQUENCY 0.15
#define MOVEMENT_VELOCITY 0.3

Tentacle::Tentacle(Jelly* jelly, Jelly::AnchorPoint anchor, const NovaColor &color) {
	this->jelly = jelly;
	this->anchor = anchor;
	nematocyst = NULL;

    // Randomize each tentacle's movement.
    phaseOffset = (float_rand(1, 4) / 4) * 6.28318f;

	// Create the segments.
    float decrement = ((float)(MAX_SEGMENT_SIZE - MIN_SEGMENT_SIZE) / NUMBER_OF_SEGMENTS);
    float currentSize = MAX_SEGMENT_SIZE;
    NovaColor currentColor = color;
    NovaColor finalColor = color;
    finalColor.rebase(30);

    for (int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
		// Make every 3rd segment explosive.
		bool explosive = false;
		if (i % 3 == 0) {
			explosive = true;
		}

		if (i < (NUMBER_OF_SEGMENTS - 1)) {
			segments.push_back(new TentacleSegment(jelly->getPlayState(), jelly, currentSize, currentColor, explosive));
		} else {
			// Add the last segment.
			nematocyst = new TentacleNematocyst(jelly->getPlayState(), jelly, currentSize, NovaColor(34, 250, 5), explosive);
			segments.push_back(nematocyst);
		}

		// Make each segment progressively smaller.
		currentSize -= decrement;

		// And darker.
		currentColor.fade((1.0 / NUMBER_OF_SEGMENTS), finalColor);
	}

    visible = false;
    disabled = false;
	totalElapsedTime = 0;
}

Tentacle::~Tentacle() {
	for (TentacleSegment* segment : segments) {
		delete segment;
	}

	segments.clear();

	// Already deleted.
	nematocyst = NULL;
}

void Tentacle::setDisabled(bool disabled) {
	for (TentacleSegment* segment : segments) {
		segment->setDisabled(disabled);
	}

	this->disabled = disabled;
}

void Tentacle::setActive(bool active) {
	if (active) {
		// Reset.
		this->setVisible(false);
		this->setDisabled(false);
		totalElapsedTime = 0;
	}

	for (TentacleSegment* segment : segments) {
		segment->setActive(active);
	}
}

void Tentacle::update(float elapsedTime) {
	totalElapsedTime += elapsedTime;

	// Position the segments.
	NovaVertex origin = jelly->getAnchorPointWCS(anchor);
	for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i) {
		float distance = (TENTACLE_LENGTH / (NUMBER_OF_SEGMENTS - 1)) * i;
		float sway = MOVEMENT_AMPLITUDE * sinf((distance * MOVEMENT_FREQUENCY) + (totalElapsedTime * MOVEMENT_VELOCITY) + phaseOffset);
        sway *= (distance / TENTACLE_LENGTH); // Taper motion near base (sway less).
        segments[i]->moveTo(origin.x + sway, origin.y - distance, 0);
	}

	for (TentacleSegment* segment : segments) {
		// Mark this segment visible/invisible for this frame.
		segment->calculateVisibility();
	}

	// If any segments are visible then mark the tentacle as visible (so the draw method is called).
	this->setVisible(false);
	for (TentacleSegment* segment : segments) {
		if (segment->isVisible()) {
			this->setVisible(true);
			break;
		}
	}


	nematocyst->update(elapsedTime);
}

bool Tentacle::checkCollision(Player *player) {
	bool collision = false;

	// See if this tentacle has just hit the player.
	for (TentacleSegment* segment : segments) {
		if (segment->isVisible()) {
			if (segment->checkCollision(player)) {
				collision = true;
				break;
			}
		}
	}

	return collision;
}

bool Tentacle::checkCollision(Missile* missile) {
	bool collision = false;

	// See if this tentacle has just hit this missile.
	for (TentacleSegment* segment : segments) {
		if (segment->isVisible()) {
			if (segment->checkCollision(missile)) {
				collision = true;
				break;
			}
		}
	}

	if (collision && (!this->isDisabled())) {
		// See if this tentacle has just been disabled.
		if (nematocyst->isDisabled()) {
			this->setDisabled(true);
		}
	}

	return collision;
}

void Tentacle::draw() {
	for (TentacleSegment* segment : segments) {
		segment->draw();
	}
}
