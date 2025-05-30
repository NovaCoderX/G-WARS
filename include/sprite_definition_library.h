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
#ifndef __SPRITE_DEFINITION_LIBRARY_H
#define __SPRITE_DEFINITION_LIBRARY_H

class SpriteDefinitionLibrary {
public:
	SpriteDefinitionLibrary();
	~SpriteDefinitionLibrary();

	void load(const std::string& filename);
	SpriteDefinition* getDefinition(const char *name);

protected:
	FILE *sceneFp;

	// Actual object instances.
	SpriteDefinition *definitions;
	int numDefinitions;

	void loadData(const char *sectionName, int &a, int &b, int &c);
	void loadData(const char *sectionName, float &a, float &b, float &c);
	void loadData(const char *sectionName, char &a);
	void loadData(const char *sectionName, float &a);
	void seekToMainSection(const char *section_name, int instance);
	void moveToMainSection(const char *section_name);
	void moveToSubSection(const char *main_section_name, const char *sub_section_name);
	void moveToOpenBrace(const char *section_name);
	int countEntries(const char *main_section_name, const char *sub_section_name, int instance);
	int countEntries();
	int countMainSections(const char *section_name);
	void loadDefinitions();
};

#endif // __SPRITE_DEFINITION_LIBRARY_H
