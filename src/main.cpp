/*Zatacka - clone of classical game achtung die Kurve
 * Copyright (C) 2005-2006 Mage <mage4@users.sourceforge.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifdef __linux__
#include <unistd.h>
#include <defaults.h>
#endif

#include "fx.h"

#ifdef WIN32
//windows need their WinMain :-/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    StoreHinstance(hInstance);
#else
int main(int argc, char* argv[] ){
        char path[30];
	sprintf(path, "%s/%s", DEFAULT_LIBDIR, PACKAGE);      	
	chdir(path);
#endif
	SDL_Surface *screen;
	screen= Initialize();
	while(true){			//main loop
		if(menu(screen)){ 	//if menu returns true
			loop(screen); 	//then start the game
		}else{
			break; 		//else exit main loop and quit program
		}
	}
	return 0;

}
