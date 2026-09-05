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

#define NUMBER_OF_ROW_VERTICES 11
#define NUMBER_OF_ROWS 7
#define NUMBER_OF_COLUMN_VERTICES 9
#define NUMBER_OF_COLUMNS 9
#define HIGH_COLOR_INTERVAL 10
#define NUMBER_OF_STROBE_COLORS 12

// The resting and peak brightness of the grid lines, as a scale factor applied to a
// fully saturated hue. Scaling (rather than adding) is what preserves the hue as the
// effect ramps up, so the sweep still reads as color at full intensity.
#define LOW_INTENSITY (16.0f / 255.0f)
#define HIGH_INTENSITY (128.0f / 255.0f)

// How much the intensity climbs per unit of elapsed time.
#define INTENSITY_RAMP_RATE 30

// How long each hue is held before the sweep advances (roughly four frames at 50Hz).
// Anything much shorter reads as static rather than as a sweep.
#define STROBE_INTERVAL 0.8

static NovaVertex savedRowPositions[NUMBER_OF_ROWS];
static NovaVertex savedColumnPositions[NUMBER_OF_COLUMNS];

PlayAreaGrid::PlayAreaGrid(PlayState* playState) {
	this->playState = playState;
	rowData.staticVertices = NULL;
	columnData.staticVertices = NULL;
	shaking = false;
	highColorTimer = 0;
	highColorDisplayed = false;
	strobeIndex = 0;
	strobeTimer = 0;
	intensity = LOW_INTENSITY;
}

PlayAreaGrid::~PlayAreaGrid() {
	if (rowData.staticVertices) {
		delete[] rowData.staticVertices;
		rowData.staticVertices  = NULL;
	}

    for (GridLine* row : rows) {
        delete row;
    }

    rows.clear();

	if (columnData.staticVertices) {
		delete[] columnData.staticVertices;
		columnData.staticVertices  = NULL;
	}

    for (GridLine* column : columns) {
        delete column;
    }

    columns.clear();
}

void PlayAreaGrid::init() {
	lowColor = NovaColor(16, 16, 16);
	lineColor = lowColor;

	// Create the rows.
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		rows.push_back(new GridLine(playState));
	}

	// Create the shared vertex row data.
	rowData.staticVertices = new NovaVertex[NUMBER_OF_ROW_VERTICES];

	int x = -100;
	for (int i = 0; i < NUMBER_OF_ROW_VERTICES; i++) {
		rowData.staticVertices[i].x = x;
		rowData.staticVertices[i].y = 0;
		rowData.staticVertices[i].z = 0;

		x += 20;
	}

	// Set the shared vertex data.
	int y = 60;
	for (int r = 0; r < NUMBER_OF_ROWS; r++) {
		rows[r]->init(NUMBER_OF_ROW_VERTICES);
		rows[r]->setColor(lineColor);

		for (int v = 0; v < NUMBER_OF_ROW_VERTICES; v++) {
			rows[r]->staticVertices[v] = &rowData.staticVertices[v];
		}

		rows[r]->moveTo(0, y, 0);

		y -= 20;
	}

	// Store the positions (for screen shaking restore).
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		savedRowPositions[i] = rows[i]->getPositionWCS();
	}


	// Create the columns.
	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
		columns.push_back(new GridLine(playState));
	}

	// Create the shared vertex column data.
	columnData.staticVertices = new NovaVertex[NUMBER_OF_COLUMN_VERTICES];

	y = -80;
	for (int i = 0; i < NUMBER_OF_COLUMN_VERTICES; i++) {
		columnData.staticVertices[i].x = 0;
		columnData.staticVertices[i].y = y;
		columnData.staticVertices[i].z = 0;

		y += 20;
	}

	// Set the shared vertex data.
	x = 80;
	for (int r = 0; r < NUMBER_OF_COLUMNS; r++) {
		columns[r]->init(NUMBER_OF_COLUMN_VERTICES);
		columns[r]->setColor(lineColor);

		for (int v = 0; v < NUMBER_OF_COLUMN_VERTICES; v++) {
			columns[r]->staticVertices[v] = &columnData.staticVertices[v];
		}

		columns[r]->moveTo(x, 0, 0);

		x -= 20;
	}

	// Store the positions (for screen shaking restore).
	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
		savedColumnPositions[i] = columns[i]->getPositionWCS();
	}
}

void PlayAreaGrid::smartBombNotification() {
	this->shaking = true;

	// Reset.
	highColorTimer = 0;
	highColorDisplayed = false;

	// Start the sweep on a random hue so that each smart bomb looks different, then
	// walk forwards from there.
	strobeIndex = int_rand(0, (NUMBER_OF_STROBE_COLORS - 1));
	strobeTimer = 0;
	intensity = LOW_INTENSITY;
}

/*
 ** Push the current line color out to every row and column.
 */
void PlayAreaGrid::applyLineColor() {
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		rows[i]->setColor(lineColor);
	}

	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
		columns[i]->setColor(lineColor);
	}
}

void PlayAreaGrid::update(float elapsedTime) {
	static NovaVertex positionCCS;
	static bool verticalSwitch = true;

	if (shaking) {
		// Advance the sweep to the next hue every few ticks.
		strobeTimer += elapsedTime;
		if (strobeTimer >= STROBE_INTERVAL) {
			strobeIndex++;
			if (strobeIndex >= NUMBER_OF_STROBE_COLORS) {
				strobeIndex = 0;
			}

			strobeTimer = 0;
		}

		// The ramp is tracked separately from the color, so that changing hue does
		// not throw away the progress made so far.
		if (!highColorDisplayed) {
			intensity += (elapsedTime / INTENSITY_RAMP_RATE);
			if (intensity >= HIGH_INTENSITY) {
				intensity = HIGH_INTENSITY;

				// Stay at the highest color for a few ticks.
				highColorDisplayed = true;
			}
		}

		// Rebuild the line color from the current hue at the current intensity.
		//lineColor = strobeColors[strobeIndex];
		lineColor = NovaColor(255, 255, 255);
		lineColor *= intensity;

		if (highColorDisplayed) {
			highColorTimer += elapsedTime;
			if (highColorTimer >= HIGH_COLOR_INTERVAL) {
				// Restore the row positions..
				for (int i = 0; i < NUMBER_OF_ROWS; i++) {
					rows[i]->moveTo(savedRowPositions[i]);
				}

				// Restore the column positions..
				for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
					columns[i]->moveTo(savedColumnPositions[i]);
				}

				// Reset.
				shaking = false;
				lineColor = lowColor;

				// Put the resting color back without disturbing the restored positions.
				this->applyLineColor();
			}
		}

		// Only shake while the effect is still running. Doing this unconditionally would
		// re-offset the rows and columns on the same frame they were restored, leaving a
		// small permanent drift behind every smart bomb.
		if (shaking) {
			// Shake it baby.
			float amount;
			if (verticalSwitch) {
				amount = float_rand(0, 3);
			} else {
				amount = -float_rand(0, 3);
			}

			for (int i = 0; i < NUMBER_OF_ROWS; i++) {
				NovaVertex position = rows[i]->getPositionWCS();
				position.y += amount;
				rows[i]->moveTo(position);
				rows[i]->setColor(lineColor);
			}

			for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
				NovaVertex position = columns[i]->getPositionWCS();
				position.x += amount;
				columns[i]->moveTo(position);
				columns[i]->setColor(lineColor);
			}

			// Switch.
			verticalSwitch = (!verticalSwitch);
		}
	}

	// Determine which rows are visible (has to be done after shaking because the positions would have changed).
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		positionCCS = playState->getCamera()->getPositionCCS(rows[i]);
		int y = verticalProject(positionCCS.y, positionCCS.z);
		if ((y >= 0) && (y < DISPLAY_HEIGHT)) {
			rows[i]->setVisible(true);
		} else {
			rows[i]->setVisible(false);
		}
	}

	// Determine which columns are visible.
	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
		positionCCS = playState->getCamera()->getPositionCCS(columns[i]);
		int x = horizontalProject(positionCCS.x, positionCCS.z);
		if ((x >= 0) && (x < DISPLAY_WITDH)) {
			columns[i]->setVisible(true);
		} else {
			columns[i]->setVisible(false);
		}
	}
}

void PlayAreaGrid::draw() {
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		if (rows[i]->isVisible()) {
			rows[i]->draw();
		}
	}

	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) {
		if (columns[i]->isVisible()) {
			columns[i]->draw();
		}
	}
}
