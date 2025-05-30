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
#ifndef __HUD_PANEL_H
#define __HUD_PANEL_H

class HudPanel {
public:
	HudPanel(PlayState* playState) {
		this->playState = playState;
		dirty = false;
		textColor = DEFAULT_TEXT_COLOR;
	}

	virtual ~HudPanel() {}

	bool isDirty() const {
		return dirty;
	}

	void setDirty(bool dirty) {
		this->dirty = dirty;
	}

protected:
	PlayState* playState;
	bool dirty;
	NovaColor textColor;
};

#endif // __HUD_PANEL_H
