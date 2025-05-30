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
#ifndef __BUTTON_H
#define __BUTTON_H

class Button: public Control {
public:
	Button(MenuState* menuState, const NovaColor& buttonColor, std::string label, const NovaColor& labelColor,
			const NovaVertex& position);

	~Button();

	// Overridden.
	void setDisabled(bool disabled);

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	void draw();

private:
	struct ButtonData {
		NovaVertex* staticVertices;
		RenderVertex* renderVertices;
		DisplayVertex* displayVertices;
		NovaColor color;
	} buttonData;

	struct BordertData {
		NovaVertex* staticVertices;
		RenderVertex* renderVertices;
		DisplayVertex* displayVertices;
		NovaColor lineColors[2];
		NovaColor highColors[2];
		NovaColor lowColors[2];
	} borderData;

	HighlightLine* borders;
	Label* label;
	NovaColor defaultLabelColor;
	NovaColor disabledLabelColor;
};

#endif // __BUTTON_H

