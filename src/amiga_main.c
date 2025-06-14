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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>

// Amiga includes.
#include <proto/dos.h>
#include <clib/icon_protos.h>
#include <workbench/startup.h>

const char *ID = "$VER: G-WARS 1.2.04\r\n";

// The startup message from workbench or 0 if started via CLI.
extern struct WBStartup *_WBenchMsg;

#define MAX_ARGVS 100
static char *myargv[MAX_ARGVS];
static int myargc = 0;


extern void nova_main(char *baseDirectory);

int main(int argc, char *argv[]) {
	char path[PATH_MAX];

    if (!_WBenchMsg) {
        // Started from the shell.
        GetCurrentDirName((unsigned char*)path, PATH_MAX);
        nova_main(path);
    } else {
		// Started from WB.
    	struct DiskObject *diskObject;
		char *toolType;

		NameFromLock(_WBenchMsg->sm_ArgList[0].wa_Lock, (unsigned char*)path, PATH_MAX);
		CurrentDir(_WBenchMsg->sm_ArgList[0].wa_Lock);

		// Setup command line arguments.
		myargv[myargc] = (char*) malloc(strlen("G-WARS") + 1);
		strcpy(myargv[myargc++], "G-WARS");

		// Process Tool types.
		diskObject = GetDiskObject((const unsigned char*) _WBenchMsg->sm_ArgList[0].wa_Name);
		if (diskObject != NULL) {
			toolType = (char*) FindToolType((const unsigned char**) diskObject->do_ToolTypes, (const unsigned char*) "closewb");
			if (toolType != NULL) {
				SDL_putenv("SDL_CLOSE_WB=1");
			}
		}

		nova_main(path);
	}

	// Never reached....
	return EXIT_SUCCESS;
}
