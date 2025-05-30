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
#ifndef __OPTION_TOGGLE_H
#define __OPTION_TOGGLE_H

class OptionToggle: public Control {
public:
	OptionToggle(MenuState* menuState, const std::string key, bool defaultValue,
			const NovaColor& textColor, const NovaVertex& position);

	~OptionToggle();

	// Overridden.
	void setDisabled(bool disabled);

	// Overridden.
	void setSelected(bool selected);

	// Overridden.
	void setInputCaptured(bool inputCaptured);

	// Overridden.
	void processInput();

	// Overridden.
	void update(float elapsedTime);

	// Overridden.
	void draw();

private:
	std::string key;
	bool currentValue;
	std::vector<CharacterSprite*> displayValue;
	NovaColor defaultTextColor;
	NovaColor disabledTextColor;
	NovaColor selectedTextColor;
	NovaColor currentTextColor;
	bool increasingColor;

	void syncCurrentValue();
	void handleKeyDown(SDL_keysym *keysym);
	void handleKeyUp(SDL_keysym *keysym);
	void moveCursorLeft();
	void moveCursorRight();
	void accept();
	void cancel();
};

#endif // __OPTION_TOGGLE_H
