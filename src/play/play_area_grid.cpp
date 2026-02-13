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

static NovaVertex savedRowPositions[NUMBER_OF_ROWS];
static NovaVertex savedColumnPositions[NUMBER_OF_COLUMNS];

PlayAreaGrid::PlayAreaGrid(PlayState* playState) {
	this->playState = playState;
	rowData.staticVertices = NULL;
	columnData.staticVertices = NULL;
	shaking = false;
	shakeTimer = 0;
	highColorTimer = 0;
	highColorDisplayed = false;
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
	highColor = NovaColor(158, 2, 12);
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
	shakeTimer = 0;
	highColorTimer = 0;
	highColorDisplayed = false;
}

void PlayAreaGrid::update(float elapsedTime) {
	static NovaVertex positionCCS;
	static bool verticalSwitch = true;

	if (shaking) {
		// Shake it baby.
		float amount;
		if (verticalSwitch) {
			amount = float_rand(0, 3);
		} else {
			amount = -float_rand(0, 3);
		}

		if (highColorDisplayed) {
			highColorTimer += elapsedTime;
			if (highColorTimer >= HIGH_COLOR_INTERVAL) {
				// Reset.
				shaking = false;
				lineColor = lowColor;
			}
		} else {
			// Do some simple color cycling.
			if (lineColor.brighten((elapsedTime / 30), highColor)) {
				// Stay at the highest color for a few ticks.
				highColorDisplayed = true;
			}
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
