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
#ifndef __CONTROL_H
#define __CONTROL_H

class Control : public MatrixObject {
public:
	Control(MenuState *menuState);

	bool isDisabled() const {
		return disabled;
	}

	virtual void setDisabled(bool disabled) {
		this->disabled = disabled;
	}

	bool isSelected() const {
		return selected;
	}

	virtual void setSelected(bool selected) {
		if (selected && (!this->disabled)) {
			this->selected = true;
		} else {
			this->selected = selected;
		}
	}

	bool isInputCaptured() const {
		return inputCaptured;
	}

	virtual void setInputCaptured(bool inputCaptured) {
		if (inputCaptured && (!this->disabled)) {
			this->inputCaptured = true;
		} else {
			this->inputCaptured = inputCaptured;
		}

		// Could notify the parent menu that we lost focus?
		//parent->lostFocus(); -> needed by music/FX disable toggle
	}

	virtual void processInput() {}
	virtual void update(float elapsedTime) {}
	virtual void draw() = 0;

protected:
	MenuState* menuState;
	bool disabled;
	bool selected;
	bool inputCaptured;
};

#endif // __CONTROL_H

