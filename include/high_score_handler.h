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
#ifndef __HIGH_SCORE_HANDLER_H
#define __HIGH_SCORE_HANDLER_H

struct HighScoreData
{
	std::string name;
	uint score;

	HighScoreData(std::string name, uint score) {
		this->name = name;
		this->score = score;
	}

	// We have to sort in decreasing order.
	bool operator< (const HighScoreData& other)
	{
		if (this->score > other.score) {
			return true;
		}

		return false;
	}
};

class HighScoreHandler {
public:
	HighScoreHandler();
	~HighScoreHandler();

	bool load(const std::string &filename);
	bool save(const std::string &filename);
	uint getHighScore();
	void addScore(uint score);
	std::vector<HighScoreData> getHighScores() { return highScores; }

private:
	std::vector<HighScoreData> highScores;
};

#endif  // __HIGH_SCORE_HANDLER_H
