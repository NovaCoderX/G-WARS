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

#define NUMBER_OF_PANEL_VERTICES 4
#define NUMBER_OF_BORDERS 4
#define BORDER_SPACING 0

MenuPanel::MenuPanel(MenuState* menuState, float width, float height, const NovaColor &panelColor, const NovaColor &borderColor,
		const NovaVertex &position) {
	this->menuState = menuState;

	// Panel setup.
	panelData.staticVertices = new NovaVertex[NUMBER_OF_PANEL_VERTICES];
	panelData.renderVertices = new RenderVertex[NUMBER_OF_PANEL_VERTICES];
	panelData.displayVertices = new DisplayVertex[NUMBER_OF_PANEL_VERTICES];

	panelData.staticVertices[0].x = -(width / 2);
	panelData.staticVertices[0].y = -(height / 2);
	panelData.staticVertices[0].z = 0;

	panelData.staticVertices[1].x = -(width / 2);
	panelData.staticVertices[1].y = (height / 2);
	panelData.staticVertices[1].z = 0;

	panelData.staticVertices[2].x = (width / 2);
	panelData.staticVertices[2].y = (height / 2);
	panelData.staticVertices[2].z = 0;

	panelData.staticVertices[3].x = (width / 2);
	panelData.staticVertices[3].y = -(height / 2);
	panelData.staticVertices[3].z = 0;

	panelData.color = panelColor;

	// Border setup.
	borderData.staticVertices = new NovaVertex[NUMBER_OF_PANEL_VERTICES];
	borderData.renderVertices = new RenderVertex[NUMBER_OF_PANEL_VERTICES];
	borderData.displayVertices = new DisplayVertex[NUMBER_OF_PANEL_VERTICES];

	borderData.staticVertices[0] = panelData.staticVertices[0];
	borderData.staticVertices[0].x -= BORDER_SPACING;
	borderData.staticVertices[0].y -= BORDER_SPACING;

	borderData.staticVertices[1] = panelData.staticVertices[1];
	borderData.staticVertices[1].x -= BORDER_SPACING;
	borderData.staticVertices[1].y += BORDER_SPACING;

	borderData.staticVertices[2] = panelData.staticVertices[2];
	borderData.staticVertices[2].x += BORDER_SPACING;
	borderData.staticVertices[2].y += BORDER_SPACING;

	borderData.staticVertices[3] = panelData.staticVertices[3];
	borderData.staticVertices[3].x += BORDER_SPACING;
	borderData.staticVertices[3].y -= BORDER_SPACING;

	borderData.color = borderColor;

	borders = new NovaLine[NUMBER_OF_BORDERS];

	borders[0].staticVertices[BEGIN] = &borderData.staticVertices[0];
	borders[0].renderVertices[BEGIN] = &borderData.renderVertices[0];
	borders[0].displayVertices[BEGIN] = &borderData.displayVertices[0];

	borders[0].staticVertices[END] = &borderData.staticVertices[1];
	borders[0].renderVertices[END] = &borderData.renderVertices[1];
	borders[0].displayVertices[END] = &borderData.displayVertices[1];

	borders[1].staticVertices[BEGIN] = &borderData.staticVertices[1];
	borders[1].renderVertices[BEGIN] = &borderData.renderVertices[1];
	borders[1].displayVertices[BEGIN] = &borderData.displayVertices[1];

	borders[1].staticVertices[END] = &borderData.staticVertices[2];
	borders[1].renderVertices[END] = &borderData.renderVertices[2];
	borders[1].displayVertices[END] = &borderData.displayVertices[2];

	borders[2].staticVertices[BEGIN] = &borderData.staticVertices[2];
	borders[2].renderVertices[BEGIN] = &borderData.renderVertices[2];
	borders[2].displayVertices[BEGIN] = &borderData.displayVertices[2];

	borders[2].staticVertices[END] = &borderData.staticVertices[3];
	borders[2].renderVertices[END] = &borderData.renderVertices[3];
	borders[2].displayVertices[END] = &borderData.displayVertices[3];

	borders[3].staticVertices[BEGIN] = &borderData.staticVertices[3];
	borders[3].renderVertices[BEGIN] = &borderData.renderVertices[3];
	borders[3].displayVertices[BEGIN] = &borderData.displayVertices[3];

	borders[3].staticVertices[END] = &borderData.staticVertices[0];
	borders[3].renderVertices[END] = &borderData.renderVertices[0];
	borders[3].displayVertices[END] = &borderData.displayVertices[0];

	this->moveTo(position);
}

MenuPanel::~MenuPanel() {
	if (panelData.staticVertices) {
		delete[] panelData.staticVertices;
		panelData.staticVertices = NULL;
	}

	if (panelData.renderVertices) {
		delete[] panelData.renderVertices;
		panelData.renderVertices = NULL;
	}

	if (panelData.displayVertices) {
		delete[] panelData.displayVertices;
		panelData.displayVertices = NULL;
	}
}

void MenuPanel::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * menuState->getCamera()->getWorldToCameraMatrix();

	// First draw the panel.
	glBegin(GL_QUADS);
	glColor3fv(panelData.color.data);

	for (int i = 0; i < NUMBER_OF_PANEL_VERTICES; i++) {
		panelData.renderVertices[i] = panelData.staticVertices[i] * objectToCameraMatrix;

		panelData.displayVertices[i].x = horizontalProject(panelData.renderVertices[i].x,
			panelData.renderVertices[i].z);
		panelData.displayVertices[i].y = verticalProject(panelData.renderVertices[i].y,
			panelData.renderVertices[i].z);

		glVertex2i(panelData.displayVertices[i].x, panelData.displayVertices[i].y);
	}
	glEnd();

	// Then draw the border, reset all (shared) scratch data.
	for (int i = 0; i < NUMBER_OF_PANEL_VERTICES; i++) {
		borderData.renderVertices[i].transformedAndProjected = false;
	}

	glBegin(GL_LINES);
	glColor3fv(borderData.color.data);
	for (int i = 0; i < NUMBER_OF_BORDERS; i++) {
		NovaLine *line = &borders[i];

		// We only transform (LCS to CCS) and project (CCS to screen coordinates) a single (shared) static vertex once per frame.
		if (!line->renderVertices[BEGIN]->transformedAndProjected) {
			// Transform.
			(*line->renderVertices[BEGIN]) = (*line->staticVertices[BEGIN]) * objectToCameraMatrix;

			// Project.
			line->displayVertices[BEGIN]->x = horizontalProject(line->renderVertices[BEGIN]->x,
					line->renderVertices[BEGIN]->z);
			line->displayVertices[BEGIN]->y = verticalProject(line->renderVertices[BEGIN]->y,
					line->renderVertices[BEGIN]->z);

			line->renderVertices[BEGIN]->transformedAndProjected = true;
		}

		if (!line->renderVertices[END]->transformedAndProjected) {
			// Transform.
			(*line->renderVertices[END]) = (*line->staticVertices[END]) * objectToCameraMatrix;

			// Project.
			line->displayVertices[END]->x = horizontalProject(line->renderVertices[END]->x,
					line->renderVertices[END]->z);
			line->displayVertices[END]->y = verticalProject(line->renderVertices[END]->y,
					line->renderVertices[END]->z);

			line->renderVertices[END]->transformedAndProjected = true;
		}

		glVertex2i(line->displayVertices[BEGIN]->x, line->displayVertices[BEGIN]->y);
		glVertex2i(line->displayVertices[END]->x, line->displayVertices[END]->y);
	}
	glEnd();
}

