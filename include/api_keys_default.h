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
#ifndef __API_KEYS_DEFAULT_H
#define __API_KEYS_DEFAULT_H

/*
 ** Placeholder values for the online high score table, so the game builds
 ** straight from the repository (online scores will simply not work).
 **
 ** For a real build, copy this file to include/api_keys.h, rename the guard
 ** and fill in the real values. api_keys.h is listed in .gitignore and is
 ** picked up automatically in place of this file when it exists.
 */
#define API_TOKEN_READ "XXX"
#define API_TOKEN_UPDATE "XXX"
#define API_TOKEN_ADMIN "XXX"

// Key used to XOR the local player data files.
#define API_SECRET_KEY "XXX"

#endif // __API_KEYS_DEFAULT_H
