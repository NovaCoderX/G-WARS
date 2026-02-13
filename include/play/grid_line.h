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
#ifndef __GRID_LINE_H
#define __GRID_LINE_H

class GridLine: public MatrixObject {
	friend class PlayAreaGrid;
public:
	GridLine(PlayState* playState);
	~GridLine();

	void setColor(const NovaColor &baseColor) {
		this->lineColor = baseColor;
	}

	void init(int numVertices);
	void draw();

private:
	PlayState* playState;

	// Shared vertex data (stored in LCS).
	NovaVertex **staticVertices;

	// Vertex data (stored in CCS).
	RenderVertex *renderVertices;

	// Vertex data (stored in VCS).
	DisplayVertex *displayVertices;

	int numVertices;
	NovaColor lineColor;
};

#endif // __GRID_LINE_H
