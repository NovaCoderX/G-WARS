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
#ifndef __EXTRA_LIFE_NUGGET_H
#define __EXTRA_LIFE_NUGGET_H

class ExtraLifeNugget: public Nugget {
public:
	ExtraLifeNugget(PlayState* playState) : Nugget(playState, EXTRA_LIFE_NUGGET, NovaColor(15, 167, 255), 80) {}
};

#endif // __EXTRA_LIFE_NUGGET_H
