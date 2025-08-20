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
#ifndef __TENTACLE_H
#define __TENTACLE_H

class Tentacle {
public:
	Tentacle(Jelly* jelly, Jelly::AnchorPoint anchor, const NovaColor &color);
	~Tentacle();

	bool isVisible() const {
		return visible;
	}

	void setVisible(bool visible) {
		this->visible = visible;
	}

	bool isDisabled() const {
		return disabled;
	}

	void setDisabled(bool disabled);
	void setActive(bool active);
	void update(float elapsedTime);
	bool checkCollision(Player* player);
	bool checkCollision(Missile* missile);
	void draw();

private:
	Jelly* jelly;
	Jelly::AnchorPoint anchor;
	std::vector<TentacleSegment*> segments;
	TentacleNematocyst* nematocyst;
    float phaseOffset;
	float totalElapsedTime;
	bool visible;
	bool disabled;
};

#endif // __TENTACLE_H
