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
#ifndef __PLAY_AREA_GRID_H
#define __PLAY_AREA_GRID_H

class PlayAreaGrid {
public:
	PlayAreaGrid(PlayState* playState);
	~PlayAreaGrid();

	void init();
	void smartBombNotification();
	void update(float elapsedTime);
	void draw();

private:
	PlayState* playState;

	struct RowVertexData {
		// Shared vertex data (stored in LCS).
		NovaVertex *staticVertices;
	} rowData;

	std::vector<GridLine*> rows;

	struct ColumnVertexData {
		// Shared vertex data (stored in LCS).
		NovaVertex *staticVertices;
	} columnData;

	std::vector<GridLine*> columns;

	bool shaking;
	float shakeTimer;
	NovaColor lineColor;
	NovaColor highColor;
	NovaColor lowColor;
	float highColorTimer;
	bool highColorDisplayed;
};

#endif // __PLAY_AREA_GRID_H

