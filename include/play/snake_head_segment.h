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
#ifndef __SNAKE_HEAD_SEGMENT_H
#define __SNAKE_HEAD_SEGMENT_H

// Forward declaration;
class Snake;

class SnakeHeadSegment: public SnakeSegment {
public:
	SnakeHeadSegment(PlayState* playState, Snake *snake, const NovaColor &highColor);
	~SnakeHeadSegment();

	// Overridden.
	void setActive(bool active);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	void youHit(Player *player);

	// Overridden.
	void youHit(Missile *missile);

	// Overridden.
	void draw();

private:
	Snake *snake;
	NovaColor highColor;
	NovaColor lowColor;

	struct TeethVertexData {
		// Vertex data (stored in LCS).
		NovaVertex *staticVertices;

		// Vertex data (stored in CCS).
		RenderVertex *renderVertices;

		// Vertex data (stored in VCS).
		DisplayVertex *displayVertices;

		NovaColor color;
	} teethData;

	struct LeftEyeVertexData {
		// Vertex data (stored in LCS).
		NovaVertex *staticVertices;

		// Vertex data (stored in CCS).
		RenderVertex *renderVertices;

		// Vertex data (stored in VCS).
		DisplayVertex *displayVertices;

		NovaColor color;
	} leftEyeData;

	struct RightEyeVertexData {
		// Vertex data (stored in LCS).
		NovaVertex *staticVertices;

		// Vertex data (stored in CCS).
		RenderVertex *renderVertices;

		// Vertex data (stored in VCS).
		DisplayVertex *displayVertices;

		NovaColor color;
	} rightEyeData;

	float totalElapsedTime;
	float lastFireTime;
};

#endif // __SNAKE_HEAD_SEGMENT_H
