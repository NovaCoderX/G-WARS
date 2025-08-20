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


SpriteDefinitionLibrary::SpriteDefinitionLibrary() {
	sceneFp = NULL;
	definitions = NULL;
	numDefinitions = 0;
}

SpriteDefinitionLibrary::~SpriteDefinitionLibrary() {
	if (sceneFp) {
		fclose(sceneFp);
		sceneFp = NULL;
	}

	if (definitions) {
		delete[] definitions;
		definitions = NULL;
	}
}

void SpriteDefinitionLibrary::load(const std::string& filename) {
	if ((sceneFp = fopen(filename.c_str(), "r")) == NULL) {
		fatalError("Could not open the sprite definition file: %s", filename.c_str());
	}

	this->loadDefinitions();

	// Close the file (no longer needed).
	if (sceneFp) {
		fclose(sceneFp);
		sceneFp = NULL;
	}
}

void SpriteDefinitionLibrary::moveToMainSection(const char *section_name) {
	char buffer[80];
	char error[255];	// Temp error msg.
	unsigned int temp;
	int ctr;
	bool found;

	found = false;

	while (!feof(sceneFp)) {
		fgets(buffer, 80, sceneFp);

		for (ctr = 0; ctr < 80; ctr++) {
			// Look for open brace.
			if (buffer[ctr] == '[') {
				found = true;

				// See if characters match.
				for (temp = 0; temp < strlen(section_name); temp++) {
					if (buffer[(ctr + 1) + temp] != section_name[temp]) {
						found = false;
						break;
					}
				}
				// Make sure there is a closing brace.
				if ((found == true) && (buffer[(ctr + 1) + temp] != ']'))
					found = false;

				// If found is still true a match was found, else continue.
				if (found)
					return;
			}  // IF.
		} // FOR.
	} // WHILE.

	if (!found) {
		strcpy(error, "Error finding main section name:");
		strcat(error, section_name);
		fatalError(error);
	}
}

void SpriteDefinitionLibrary::seekToMainSection(const char *section_name, int instance) {
	int ctr;

	rewind(sceneFp);	// Start at the start of file.

	// Move down file to correct occurrence of main-section.
	for (ctr = 0; ctr <= instance; ctr++) {
		moveToMainSection(section_name);
	}
}

void SpriteDefinitionLibrary::moveToOpenBrace(const char *section_name) {
	char buffer[80];
	int ctr;

	while (!feof(sceneFp)) {
		fgets(buffer, 80, sceneFp);

		for (ctr = 0; ctr < 80; ctr++) {
			if (buffer[ctr] == '\n' || buffer[ctr] == 0)
				break;

			if (buffer[ctr] == '{')
				return;	// Success.

			// Make sure we don't go past this section.
			switch (buffer[ctr]) {
			case '}':
				fatalError("Found a closing brace, when looking for opening in section: %s", section_name);
				break;

			case '<':
				fatalError("Found an opening sub-heading, when looking for opening brace in section: %s", section_name);
				break;

			case '>':
				fatalError("Found an closing sub-heading, when looking for opening brace in section: %s", section_name);
				break;

			case '[':
				fatalError("Found an opening main-heading, when looking for opening brace in section: %s", section_name);
				break;

			case ']':
				fatalError("Found an closing main-heading, when looking for opening brace in section: %s", section_name);
				break;
			}
		} // FOR.
	} // WHILE.

	fatalError("Could not find an opening brace in section: %s", section_name);
}

void SpriteDefinitionLibrary::moveToSubSection(const char *main_section_name, const char *sub_section_name) {
	int ctr;
	unsigned int temp;
	bool found;
	char buffer[80];
	char error[255];

	found = false;

	while (!feof(sceneFp)) {
		fgets(buffer, 80, sceneFp);

		for (ctr = 0; ctr < 80; ctr++) {
			// If we hit a carriage return give up, as rest of buffer contains rubbish!
			if (buffer[ctr] == '\n' || buffer[ctr] == 0)
				break;

			// Look for opening brace.
			if (buffer[ctr] == '<') {
				found = true;

				// See if characters match.
				for (temp = 0; temp < strlen(sub_section_name); temp++) {
					if (buffer[(ctr + 1) + temp] != sub_section_name[temp]) {
						found = false;
						break;
					}
				}
				// Make sure there is a closing brace.
				if ((found == true) && (buffer[(ctr + 1) + temp] != '>'))
					found = false;

				// If found is still true, a match was found, else continue.
				if (found)
					return;
			}  // IF.

			// Make sure we don't go past this section.
			switch (buffer[ctr]) {
			case '}':
				strcpy(error, "Could not find sub-heading name:");
				strcat(error, sub_section_name);
				strcat(error, ", in main-section:");
				strcat(error, main_section_name);
				fatalError(error);
				break;

			case '{':
				strcpy(error, "Found an opening brace, when looking for sub-heading:");
				strcat(error, sub_section_name);
				strcat(error, ", in main-section:");
				strcat(error, main_section_name);
				fatalError(error);
				break;

			case '[':
				strcpy(error, "Found an opening main-heading, when looking for sub-heading:");
				strcat(error, sub_section_name);
				strcat(error, ", in main-section:");
				strcat(error, main_section_name);
				fatalError(error);
				break;

			case ']':
				strcpy(error, "Found an closing main-heading, when looking for sub-heading:");
				strcat(error, sub_section_name);
				strcat(error, ", in main-section:");
				strcat(error, main_section_name);
				fatalError(error);
				break;
			}
		} // FOR.
	} // WHILE.

	if (!found) {
		strcpy(error, "Could not find sub-heading name:");
		strcat(error, sub_section_name);
		strcat(error, ", in main-section:");
		strcat(error, main_section_name);
		fatalError(error);
	}
}

int SpriteDefinitionLibrary::countEntries(const char *main_section_name, const char *sub_section_name, int instance) {
	// Counts the number of entries of a specfic instance of a definition or obj.
	int ctr;

	rewind(sceneFp);	// Start at the start of file.

	// Move down file to correct occurrence of main-section.
	for (ctr = 0; ctr <= instance; ctr++)
		moveToMainSection(main_section_name);

	moveToOpenBrace(main_section_name);
	moveToSubSection(main_section_name, sub_section_name);

	// We are now in the correct place to count entries.
	return countEntries();
}

int SpriteDefinitionLibrary::countEntries(void) {
	char buffer[80];
	int sum = 0;

	/*
	 ** Count each line that does not BEGIN with '[', '\n', or is
	 ** NULL as a valid entry, even thought it may not be.
	 */
	while (!feof(sceneFp)) {
		if (fgets(buffer, 80, sceneFp)) {
			if (buffer[1] == '<' || buffer[0] == '\n' || buffer[0] == '[' || buffer[0] == '}')
				break;
			else
				sum++;
		} else {
			break;
		}
	}

	return sum;
}

int SpriteDefinitionLibrary::countMainSections(const char *section_name) {
	char buffer[80];
	int ctr;
	unsigned int temp;
	bool found;
	int sum;

	sum = 0;

	rewind(sceneFp);

	while (!feof(sceneFp)) {
		fgets(buffer, 80, sceneFp);

		for (ctr = 0; ctr < 80; ctr++) {
			// Look for open brace.
			if (buffer[ctr] == '[') {
				found = true;

				// See if characters match.
				for (temp = 0; temp < strlen(section_name); temp++) {
					if (buffer[(ctr + 1) + temp] != section_name[temp]) {
						found = false;
						break;
					}
				}

				// Make sure there is a closing brace.
				if ((found == true) && (buffer[(ctr + 1) + temp] != ']'))
					found = false;

				// If found is still true a match was found, else continue.
				if (found)
					sum++;
			}  // IF.
		} // FOR.
	} // WHILE.

	return sum;
}

SpriteDefinition *SpriteDefinitionLibrary::getDefinition(const char *name) {
	int ctr;
	SpriteDefinition *definition = NULL;

	for (ctr = 0; ctr < numDefinitions; ctr++) {
		if ((strcmp(name, definitions[ctr].getName())) == 0) {
			definition = &definitions[ctr];
		}
	}

	if (!definition) {
		char error[255];

		strcpy(error, "Could not find definition: ");
		strcat(error, name);
		strcat(error, ", in GetDefinition()");
		fatalError(error);
	}

	return definition;
}
/*****************************************************************************/
void SpriteDefinitionLibrary::loadData(const char *sectionName, int &a, int &b, int &c) {
	// Loads a row of data into the specified pointers.
	// The disk file must already be in the right place.
	bool loaded = false;
	char buffer[80];
	char error[255];

	if (fgets(buffer, 80, sceneFp) != NULL)
		if ((buffer[0] != '}') || (buffer[0] != '[') || (buffer[0] != ']'))
			if (buffer[0] != 0 && buffer[0] != '\n') {
				sscanf(buffer, "%d,%d,%d", &a, &b, &c);
				loaded = true;
			}

	if (!loaded) {
		strcpy(error, "Could not load data for: ");
		strcat(error, sectionName);
		strcat(error, ", in LoadData()");
		fatalError(error);
	}
}

void SpriteDefinitionLibrary::loadData(const char *sectionName, float &a, float &b, float &c) {
	bool loaded = false;
	char buffer[80];
	char error[255];

	if (fgets(buffer, 80, sceneFp) != NULL)
		if ((buffer[0] != '}') || (buffer[0] != '[') || (buffer[0] != ']'))
			if (buffer[0] != 0 && buffer[0] != '\n') {
				sscanf(buffer, "%f,%f,%f", &a, &b, &c);
				loaded = true;
			}

	if (!loaded) {
		strcpy(error, "Could not load data for: ");
		strcat(error, sectionName);
		strcat(error, ", in LoadData()");
		fatalError(error);
	}
}

void SpriteDefinitionLibrary::loadData(const char *sectionName, char &a) {
	bool loaded = false;
	char buffer[80];
	char error[255];

	if (fgets(buffer, 80, sceneFp) != NULL)
		if ((buffer[0] != '}') || (buffer[0] != '[') || (buffer[0] != ']'))
			if (buffer[0] != 0 && buffer[0] != '\n') {
				sscanf(buffer, "%s", &a);
				loaded = true;
			}

	if (!loaded) {
		strcpy(error, "Could not load data for: ");
		strcat(error, sectionName);
		strcat(error, ", in LoadData()");
		fatalError(error);
	}
}

void SpriteDefinitionLibrary::loadData(const char *sectionName, float &a) {
	bool loaded = false;
	char buffer[80];
	char error[255];

	if (fgets(buffer, 80, sceneFp) != NULL)
		if ((buffer[0] != '}') || (buffer[0] != '[') || (buffer[0] != ']'))
			if (buffer[0] != 0 && buffer[0] != '\n') {
				sscanf(buffer, "%f", &a);
				loaded = true;
			}

	if (!loaded) {
		strcpy(error, "Could not load data for: ");
		strcat(error, sectionName);
		strcat(error, ", in LoadData()");
		fatalError(error);
	}
}
/******************************************************************************/
void SpriteDefinitionLibrary::loadDefinitions() {
	int ctr, inCtr;
	float x, y, z;
	int startVertex, endVertex;
	int numVertices, numLines;
	char name[20];
	char buffer[80];
	char mainSection[] = { "definition" };
	float factor;

	numDefinitions = countMainSections(mainSection);
	if (numDefinitions == 0) {
		fatalError("Could not find any object definitions");
	}

	// Create local instance.
	definitions = new SpriteDefinition[numDefinitions];

	// Initialize.
	for (ctr = 0; ctr < numDefinitions; ctr++) {
		seekToMainSection(mainSection, ctr);
		moveToOpenBrace(mainSection);
		moveToSubSection(mainSection, "name");
		loadData(mainSection, name[0]);
		definitions[ctr].setName(name);

		// Find out how many vertices 'this' object has. [rewinds file]
		numVertices = countEntries(mainSection, "vertices", ctr);
		if (numVertices == 0) {
			fatalError("No vertices found");
			return;
		}

		// Instantiate vertex array.
		definitions[ctr].allocateVertices(numVertices);
		seekToMainSection(mainSection, ctr);
		moveToOpenBrace(mainSection);
		moveToSubSection(mainSection, "vertices");

		// Load vertex data.
		for (inCtr = 0; inCtr < numVertices; inCtr++) {
			if (fgets(buffer, 80, sceneFp) != NULL) {
				sscanf(buffer, "%f,%f,%f", &x, &y, &z);
				definitions[ctr].setVertex(inCtr, x, y, z);
			}
		}

		// Find out how many lines 'this' object has. [rewinds file]
		numLines = countEntries(mainSection, "lines", ctr);
		if (numLines == 0) {
			fatalError("No lines found");
			return;
		}

		// Instantiate line array.
		definitions[ctr].allocateLines(numLines);
		seekToMainSection(mainSection, ctr);
		moveToOpenBrace(mainSection);
		moveToSubSection(mainSection, "lines");

		// Load line data.
		for (inCtr = 0; inCtr < numLines; inCtr++) {
			if (fgets(buffer, 80, sceneFp) != NULL) {
				sscanf(buffer, "%u,%u", &startVertex, &endVertex);
				definitions[ctr].setLineVertexMapping(inCtr, startVertex, endVertex);
			}
		}

		moveToSubSection(mainSection, "scaling");
		loadData(mainSection, factor);
		definitions[ctr].scale(factor);

		definitions[ctr].calculateBoundingSphere();
	} // FOR NUM_DEFS.
}



