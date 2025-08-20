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

#define BUTTON_WIDTH 62
#define BUTTON_HEIGHT 10
#define NUMBER_OF_VERTICES 4
#define NUMBER_OF_BORDERS 4
#define BORDER_SPACING 0

Button::Button(MenuState* menuState, const NovaColor& buttonColor, std::string label, const NovaColor& labelColor,
		const NovaVertex& position) : Control(menuState) {
	// Button data setup.
	buttonData.staticVertices = new NovaVertex[NUMBER_OF_VERTICES];
	buttonData.renderVertices = new RenderVertex[NUMBER_OF_VERTICES];
	buttonData.displayVertices = new DisplayVertex[NUMBER_OF_VERTICES];

	buttonData.staticVertices[0].x = -(BUTTON_WIDTH / 2);
	buttonData.staticVertices[0].y = -(BUTTON_HEIGHT / 2);
	buttonData.staticVertices[0].z = 0;

	buttonData.staticVertices[1].x = -(BUTTON_WIDTH / 2);
	buttonData.staticVertices[1].y = (BUTTON_HEIGHT / 2);
	buttonData.staticVertices[1].z = 0;

	buttonData.staticVertices[2].x = (BUTTON_WIDTH / 2);
	buttonData.staticVertices[2].y = (BUTTON_HEIGHT / 2);
	buttonData.staticVertices[2].z = 0;

	buttonData.staticVertices[3].x = (BUTTON_WIDTH / 2);
	buttonData.staticVertices[3].y = -(BUTTON_HEIGHT / 2);
	buttonData.staticVertices[3].z = 0;

	buttonData.color = buttonColor;

	// Border setup.
	borderData.staticVertices = new NovaVertex[NUMBER_OF_VERTICES];
	borderData.renderVertices = new RenderVertex[NUMBER_OF_VERTICES];
	borderData.displayVertices = new DisplayVertex[NUMBER_OF_VERTICES];

	borderData.staticVertices[0] = buttonData.staticVertices[0];
	borderData.staticVertices[0].x -= BORDER_SPACING;
	borderData.staticVertices[0].y -= BORDER_SPACING;

	borderData.staticVertices[1] = buttonData.staticVertices[1];
	borderData.staticVertices[1].x -= BORDER_SPACING;
	borderData.staticVertices[1].y += BORDER_SPACING;

	borderData.staticVertices[2] = buttonData.staticVertices[2];
	borderData.staticVertices[2].x += BORDER_SPACING;
	borderData.staticVertices[2].y += BORDER_SPACING;

	borderData.staticVertices[3] = buttonData.staticVertices[3];
	borderData.staticVertices[3].x += BORDER_SPACING;
	borderData.staticVertices[3].y -= BORDER_SPACING;

	// All of the border lines share the same two colors.
	borderData.lowColors[0] = NovaColor(25, 25, 112);
	borderData.lowColors[1] = NovaColor(136, 8, 8);
	borderData.highColors[0] = NovaColor(168, 168, 255);
	borderData.highColors[1] = NovaColor(255, 127, 127);
	borderData.lineColors[0] = borderData.lowColors[0];
	borderData.lineColors[1] = borderData.lowColors[1];

	borders = new HighlightLine[NUMBER_OF_BORDERS];
	borders[0].staticVertices[LINE_BEGIN] = &borderData.staticVertices[0];
	borders[0].renderVertices[LINE_BEGIN] = &borderData.renderVertices[0];
	borders[0].displayVertices[LINE_BEGIN] = &borderData.displayVertices[0];
	borders[0].lineColors[LINE_BEGIN] = &borderData.lineColors[0];

	borders[0].staticVertices[LINE_END] = &borderData.staticVertices[1];
	borders[0].renderVertices[LINE_END] = &borderData.renderVertices[1];
	borders[0].displayVertices[LINE_END] = &borderData.displayVertices[1];
	borders[0].lineColors[LINE_END] = &borderData.lineColors[1];

	borders[1].staticVertices[LINE_BEGIN] = &borderData.staticVertices[1];
	borders[1].renderVertices[LINE_BEGIN] = &borderData.renderVertices[1];
	borders[1].displayVertices[LINE_BEGIN] = &borderData.displayVertices[1];
	borders[1].lineColors[LINE_BEGIN] = &borderData.lineColors[1];

	borders[1].staticVertices[LINE_END] = &borderData.staticVertices[2];
	borders[1].renderVertices[LINE_END] = &borderData.renderVertices[2];
	borders[1].displayVertices[LINE_END] = &borderData.displayVertices[2];
	borders[1].lineColors[LINE_END] = &borderData.lineColors[0];

	borders[2].staticVertices[LINE_BEGIN] = &borderData.staticVertices[2];
	borders[2].renderVertices[LINE_BEGIN] = &borderData.renderVertices[2];
	borders[2].displayVertices[LINE_BEGIN] = &borderData.displayVertices[2];
	borders[2].lineColors[LINE_BEGIN] = &borderData.lineColors[0];

	borders[2].staticVertices[LINE_END] = &borderData.staticVertices[3];
	borders[2].renderVertices[LINE_END] = &borderData.renderVertices[3];
	borders[2].displayVertices[LINE_END] = &borderData.displayVertices[3];
	borders[2].lineColors[LINE_END] = &borderData.lineColors[1];

	borders[3].staticVertices[LINE_BEGIN] = &borderData.staticVertices[3];
	borders[3].renderVertices[LINE_BEGIN] = &borderData.renderVertices[3];
	borders[3].displayVertices[LINE_BEGIN] = &borderData.displayVertices[3];
	borders[3].lineColors[LINE_BEGIN] = &borderData.lineColors[1];

	borders[3].staticVertices[LINE_END] = &borderData.staticVertices[0];
	borders[3].renderVertices[LINE_END] = &borderData.renderVertices[0];
	borders[3].displayVertices[LINE_END] = &borderData.displayVertices[0];
	borders[3].lineColors[LINE_END] = &borderData.lineColors[0];

	this->moveTo(position);

	// Label setup.
	defaultLabelColor = labelColor;
	disabledLabelColor = defaultLabelColor;
	disabledLabelColor.rebase(50);
	this->label = new Label(menuState, label, defaultLabelColor, CENTERED_LABEL, position);
}

Button::~Button() {
	if (buttonData.staticVertices) {
		delete[] buttonData.staticVertices;
		buttonData.staticVertices = NULL;
	}

	if (buttonData.renderVertices) {
		delete[] buttonData.renderVertices;
		buttonData.renderVertices = NULL;
	}

	if (buttonData.displayVertices) {
		delete[] buttonData.displayVertices;
		buttonData.displayVertices = NULL;
	}

	if (borderData.staticVertices) {
		delete[] borderData.staticVertices;
		borderData.staticVertices = NULL;
	}

	if (borderData.renderVertices) {
		delete[] borderData.renderVertices;
		borderData.renderVertices = NULL;
	}

	if (borderData.displayVertices) {
		delete[] borderData.displayVertices;
		borderData.displayVertices = NULL;
	}

	if (borders) {
		delete[] borders;
		borders = NULL;
	}

	if (label) {
		delete label;
		label = NULL;
	}
}

void Button::setDisabled(bool disabled) {
	// Base processing.
	Control::setDisabled(disabled);

	if (this->isDisabled()) {
		label->setColor(disabledLabelColor);
	} else {
		label->setColor(defaultLabelColor);
	}
}

void Button::update(float elapsedTime) {
	if (this->isSelected()) {
		// Color cycle the selected surround.
		for (int i = 0; i < 2; i++) {
			if (borderData.lineColors[i].brighten((elapsedTime / 10), borderData.highColors[i])) {
				borderData.lineColors[i] = borderData.lowColors[i];
			}
		}
	}
}

void Button::draw() {
	static AstMatrix4x3 objectToCameraMatrix;

	// Update our composite LCS to CCS matrix that consists of the product of the object to world matrix and the
	// global world to camera matrix.
	objectToCameraMatrix = objectToWorldMatrix * menuState->getCamera()->getWorldToCameraMatrix();

	// First draw the button.
	glBegin(GL_QUADS);
	glColor3fv(buttonData.color.data);

	for (int i = 0; i < NUMBER_OF_VERTICES; i++) {
		buttonData.renderVertices[i] = buttonData.staticVertices[i] * objectToCameraMatrix;

		buttonData.displayVertices[i].x = horizontalProject(buttonData.renderVertices[i].x,
				buttonData.renderVertices[i].z);
		buttonData.displayVertices[i].y = verticalProject(buttonData.renderVertices[i].y,
				buttonData.renderVertices[i].z);

		glVertex2i(buttonData.displayVertices[i].x, buttonData.displayVertices[i].y);
	}
	glEnd();

	// Then draw the button label.
	glBegin(GL_LINES);
	label->draw();
	glEnd();

	// Then draw the selected surround.
	if (this->isSelected()) {
		// Reset all (shared) scratch data used by the border.
		for (int i = 0; i < NUMBER_OF_VERTICES; i++) {
			borderData.renderVertices[i].transformedAndProjected = false;
		}

		glLineWidth(3.0);
		glBegin(GL_LINES);

		for (int i = 0; i < NUMBER_OF_BORDERS; i++) {
			HighlightLine *line = &borders[i];

			// We only transform (LCS to CCS) and project (CCS to screen coordinates) a single (shared) static vertex once per frame.
			if (!line->renderVertices[LINE_BEGIN]->transformedAndProjected) {
				// Transform.
				(*line->renderVertices[LINE_BEGIN]) = (*line->staticVertices[LINE_BEGIN]) * objectToCameraMatrix;

				// Project.
				line->displayVertices[LINE_BEGIN]->x = horizontalProject(line->renderVertices[LINE_BEGIN]->x,
						line->renderVertices[LINE_BEGIN]->z);
				line->displayVertices[LINE_BEGIN]->y = verticalProject(line->renderVertices[LINE_BEGIN]->y,
						line->renderVertices[LINE_BEGIN]->z);

				line->renderVertices[LINE_BEGIN]->transformedAndProjected = true;
			}

			if (!line->renderVertices[LINE_END]->transformedAndProjected) {
				// Transform.
				(*line->renderVertices[LINE_END]) = (*line->staticVertices[LINE_END]) * objectToCameraMatrix;

				// Project.
				line->displayVertices[LINE_END]->x = horizontalProject(line->renderVertices[LINE_END]->x,
						line->renderVertices[LINE_END]->z);
				line->displayVertices[LINE_END]->y = verticalProject(line->renderVertices[LINE_END]->y,
						line->renderVertices[LINE_END]->z);

				line->renderVertices[LINE_END]->transformedAndProjected = true;
			}

			glColor3fv(line->lineColors[LINE_BEGIN]->data);
			glVertex2i(line->displayVertices[LINE_BEGIN]->x, line->displayVertices[LINE_BEGIN]->y);

			glColor3fv(line->lineColors[LINE_END]->data);
			glVertex2i(line->displayVertices[LINE_END]->x, line->displayVertices[LINE_END]->y);
		}

		glEnd();
		glLineWidth(1.0);
	}
}

