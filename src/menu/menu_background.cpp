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

#define MAX_STARS 200
#define MAX_POS 10000
#define MAX_ANGLES 6000

enum {
	STREAK = 0, CIRCLE = 1
};

typedef struct _starRec {
	GLint type;
	float x, y, z;
} starRec;

static starRec stars[MAX_STARS];
static float sinTable[MAX_ANGLES];

static inline float cosLookup(float angle) {
    return (sinTable[(GLint)angle]);
}

static inline float sinLookup(float angle) {
    return (sinTable[((GLint)angle+(MAX_ANGLES/4))%MAX_ANGLES]);
}

MenuBackground::MenuBackground(MenuState *menuState) {
	this->menuState = menuState;
}

MenuBackground::~MenuBackground() {
	// Empty.
}

static void createStar(int index) {
	if (rand() % 4 == 0) {
		stars[index].type = CIRCLE;
	} else {
		stars[index].type = STREAK;
	}

	stars[index].x = (float) (rand() % MAX_POS - MAX_POS / 2);
	stars[index].y = (float) (rand() % MAX_POS - MAX_POS / 2);
	stars[index].z = (float) (rand() % MAX_POS);
}

void MenuBackground::init() {
	float angle = 0.0;

	for (int i = 0; i < MAX_ANGLES; i++) {
		sinTable[i] = sin(angle);
		angle += M_PI / (MAX_ANGLES / 2.0);
	}

	for (int i = 0; i < MAX_STARS; i++) {
		createStar(i);
	}
}

void MenuBackground::update(float elapsedTime) {
	float offset = (120 * elapsedTime);

	for (int i = 0; i < MAX_STARS; i++) {
		stars[i].z -= offset;
	}
}

void MenuBackground::draw() {
	float x0, y0, width;
	float intensity;

	for (int n = 0; n < MAX_STARS; n++) {
		x0 = stars[n].x * DISPLAY_WITDH / stars[n].z;
		y0 = stars[n].y * DISPLAY_HEIGHT / stars[n].z;
		x0 += DISPLAY_WITDH / 2.0;
		y0 += DISPLAY_HEIGHT / 2.0;

		intensity = 1 - (stars[n].z * (1.0 / MAX_POS));

		if (x0 >= 0.0 && x0 < DISPLAY_WITDH && y0 >= 0.0 && y0 < DISPLAY_HEIGHT) {
			if (stars[n].type == STREAK) {
				glLineWidth(MAX_POS / 100.0 / stars[n].z + 1.0);
				glColor3f(intensity, intensity, intensity);
				glBegin(GL_POINTS);
				glVertex2f(x0, y0);
				glEnd();
			} else {
				width = MAX_POS / 10.0 / stars[n].z + 1.0;
				glColor3f(intensity, intensity, intensity);
				glBegin(GL_POLYGON);
				for (int i = 0; i < 8; i++) {
					float x = x0 + width * cosLookup((float) i * MAX_ANGLES / 8.0);
					float y = y0 + width * sinLookup((float) i * MAX_ANGLES / 8.0);
					glVertex2f(x, y);
				}
				glEnd();
			}
		} else {
			// Moved out of bounds (no longer visible).
			createStar(n);
		}
	}
}

