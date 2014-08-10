/*Zatacka - clone of classical game achtung die Kurve
 *  Copyright (C) 2005-2006 Mage <mage4@users.sourceforge.net>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *  
 */


#ifndef FX_H
#define FX_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "SDL_prim.h"
#include <SDL/SDL_ttf.h>

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <dirent.h>

#ifdef WIN32
#include <windows.h>
#include <dir.h>
#endif

#define W_CAPTION "Zatacka"
#define XRES 640
#define YRES 480
#define MAXPL 6
#define MAXROUNDS 10
#define FONT "font.ttf"

SDL_Surface* Initialize(); /*initialization of game*/
void disperr(char * text); /*desplay an error*/
void loop(SDL_Surface *screen); /*game loop*/
bool menu(SDL_Surface *screen); /*menu loop*/
void DisplayImage(SDL_Surface *image,SDL_Surface *screen,int x,int y); 
SDL_Surface* LoadImage(char *file); /*load image from file*/
Uint8 GetPixelColor (SDL_Surface *surface,int x,int y);
void DisplayScore(SDL_Surface *screen);
int irand(int max); /*random integer number*/
char* GetAppPath();
void DispMenu(SDL_Surface *screen);
void PlaceBarriers(SDL_Surface *screen,int count);
void DispText(SDL_Surface *screen,SDL_Color color,int x, int y, int w, int h, char *text);
#ifdef WIN32
void StoreHinstance(HINSTANCE hi);
#endif

class line {
	public:
		line();
		~line(){}
		void Reset();
		void SetColor(int r,int g, int b);
		void SetDest(SDL_Surface *ndest){dest=ndest;}
		SDL_Color GetColor(){return color;}
		void ChangeDirection(bool plus);
		void Move(bool space);
		void IncScore(){score++;}
		int GetScore(){return score;}
		void ResetScore(){score=0;}
		bool left;
		bool right;
		bool dead;
		bool mouse;
		SDLKey lkey;
		SDLKey rkey;
	private:
		int score;
		int x;		/*aktualni pozice*/
		int y;
		int direction; //koeficient smeru
		SDL_Color color;
		SDL_Surface *dest;
};

#endif
