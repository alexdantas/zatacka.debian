/*Zatacka - clone of classical game achtung die Kurve
 * Copyright (C) 2005-2006 Mage <mage4@users.sourceforge.net>
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
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *           
*/

#include "fx.h"

line pline[6]; //6 players max
int alivecnt=0;
bool barriers;
int barcount=10;
char bgpath[100]; //path to background image (map)
//direction coef.
int cx[]={0,1,2,3,4,5,4,3,2,1,0,-1,-2,-3,-4,-5,-4,-3,-2,-1};
int cy[]={-5,-4,-3,-2,-1,0,1,2,3,4,5,4,3,2,1,0,-1,-2,-3,-4};
SDL_Surface *bg;
int xres=XRES, yres=YRES; //resolution variables, setting to default resolution
TTF_Font *font;
int speed=5;

#ifdef WIN32
HINSTANCE hInstance;
#endif

void disperr(char * text){
#ifdef WIN32
	//on Windows use their MessageBox API to display error message
	MessageBox(NULL,text,"Zatacka error",MB_ICONERROR);
#else
	fprintf(stderr,"%s\n",text);		//on non Win32 system just print message to stderr
#endif
	exit(1);
}

SDL_Surface *Initialize(){
	SDL_Surface *screen;
	int i;
	char* wd;

	//wd=GetAppPath(); //get path to application directory
	//chdir(wd); //change working directory to application directory

	if((SDL_Init(SDL_INIT_VIDEO ))==-1){ //initialize SDL library
		disperr("Error initializing SDL");
	}
	atexit(SDL_Quit); //at exit call SDL_Quit
	SDL_WM_SetCaption(W_CAPTION,NULL); //set window caption
	
	screen=SDL_SetVideoMode(XRES,YRES,16,SDL_HWSURFACE); //create screen surface
	if(screen==NULL){
		disperr("Could not set video mode");
		exit(1);
	}
	
	TTF_Init(); //SDL_ttf init
	atexit(TTF_Quit); //at exit call TTF_quit

	for(i=0;i<6;i++){ 
		pline[i].SetDest(screen);
	}
	//set color of each line
	pline[0].SetColor(50,50,255);
	pline[1].SetColor(255,10,10);
	pline[2].SetColor(255,255,0);
	pline[3].SetColor(0,255,0);
	pline[4].SetColor(255,0,255);
	pline[5].SetColor(255,255,255);
	bg=LoadImage("main_screen.jpg"); //load menu backgrount into SDL_Surface
	font=TTF_OpenFont(FONT,20);
	return (screen); //return pointer to screen surface
	
	
}

//the game loop
void loop(SDL_Surface *screen){
	SDL_Event event;
	SDL_Event event2;
	bool quit=false;
	bool quit2=false;
	bool pau=false; //pause
	int i=0;
	int j;
	int oldalivecnt; //original count of players
	int roundcnt=0; //count of rounds
	Uint32 flags;
	SDL_Surface *bgimage;	//background image
	char tmppath[110];
	Uint32 borderclr=SDL_MapRGB(screen->format,128,128,0);
	
	if((strcmp(bgpath,"none"))){	//if I have a map
		strcpy(tmppath,"maps/");
		strcat(tmppath,bgpath);
		bgimage=LoadImage(tmppath);
		xres=bgimage->clip_rect.w + 40; //need some place for score bar
		yres=bgimage->clip_rect.h;
		barriers=false; //no random barriers when I have map
	}
	flags=screen->flags;
	if(((4*yres)/xres)!=3){ 	//if resolution isn't 4:3
		flags = !(flags | SDL_FULLSCREEN) ; //can't use fulscreen mode
	}
	screen=SDL_SetVideoMode(xres,yres,16,flags);
	if(strcmp(bgpath,"none")){	//if I have background image
		DisplayImage(bgimage,screen,0,0);
	}else{
		SDL_FillRect(screen,NULL,0);
	}
	//draw the border that the players must not hit
	SDL_drawLine(screen, 5,      5,      xres-5, 5,      borderclr);
	SDL_drawLine(screen, xres-5, 5,      xres-5, yres-5, borderclr);
	SDL_drawLine(screen, xres-5, yres-5, 5,      yres-5, borderclr);
	SDL_drawLine(screen, 5,      yres-5, 5,      5,      borderclr);  
	SDL_drawLine(screen, xres-40,5,	xres-40,yres-5,	borderclr); 
	SDL_Flip(screen);
	if(barriers)
		PlaceBarriers(screen,barcount);
	DisplayScore(screen); //display zero score

	alivecnt=0; //zero count of alives
	for(j=0;j<MAXPL;j++){  //how many players do i have?
		if(pline[j].dead==false){
			alivecnt++;
		}
	}
	if(alivecnt<2) //there must be 2 or more players for a game
		return;
	oldalivecnt=alivecnt; //store original count of players
      while(!quit){ //game loop
	while(SDL_PollEvent(&event)){ //event loop
		if(event.type==SDL_KEYDOWN){	//control keys
			switch(event.key.keysym.sym){
				case SDLK_ESCAPE:	//escape = exit
					quit=true;
					break;
				case SDLK_SPACE:
					pau=!pau;	//space = pause
			}
		}
		for(j=0;j<MAXPL;j++){  //catch all lines
		    if(!pline[j].mouse){ //do I handle by mouse?
			switch(event.type){ //if not, I handle by keyboard
				case SDL_KEYDOWN: //when key is pressed
					if(event.key.keysym.sym==pline[j].lkey){ 	 //start turning left
						pline[j].left=true;
						break;
					}
					if(event.key.keysym.sym==pline[j].rkey){	//start turning right
						pline[j].right=true;
						break;
					}
					break;
				case SDL_KEYUP: //when key is released
					if(event.key.keysym.sym==pline[j].lkey){	//stop turning left
						pline[j].left=false;
						break;
					}
					if(event.key.keysym.sym==pline[j].rkey){	//stop turning right
						pline[j].right=false;
					}
					break;
			}
		    }else{	//I handle by mouse
			    switch(event.type){
				case SDL_MOUSEBUTTONDOWN:	//button is pressed
					if(event.button.button==SDL_BUTTON_LEFT){
						pline[j].left=true;
						break;
					}
					if(event.button.button==SDL_BUTTON_RIGHT){
						pline[j].right=true;
					}
					break;
				case SDL_MOUSEBUTTONUP:		//button is released
					if(event.button.button==SDL_BUTTON_LEFT){
						pline[j].left=false;
						break;
					}
					if(event.button.button==SDL_BUTTON_RIGHT){
						pline[j].right=false;
					}
					break;
			    }
		        
				
			}
		}
	}
      if(!pau){	//if game isn't paused
	i++;	//tic counter
        for(j=0;j<MAXPL;j++){ 		//for each player
	 if(!pline[j].dead){ 		//for each alive player
	  if(pline[j].left){ 		//if left key is pressed
		  pline[j].ChangeDirection(false); //turn left`
	  }
	  if(pline[j].right){		//if right key is pressed
		  pline[j].ChangeDirection(true); //turn right
	  }
	/*create hole every 30 times*/
	  if(i>30){
		pline[j].Move(true);
	  }else{
	        pline[j].Move(false);
	  }
		  
   	 } 
        }
	if(i>30){
		i=0;
	}
	if(alivecnt <2){ //increse count of the rounds when rount is going to end
		roundcnt++;
	}
	if(roundcnt>MAXROUNDS){
		//display game over
		DispText(screen,pline[0].GetColor(),xres/2-50,yres/2-40,100,20,"Game over");
		DispText(screen,pline[1].GetColor(),xres/2-200,yres/2,200,20,"(SPACE) Play again    (ESCAPE) Return to menu");
		while(!quit2){
		  SDL_WaitEvent(&event2);
		  if(event2.type==SDL_KEYDOWN){
			switch(event2.key.keysym.sym){
				case SDLK_SPACE:
					//play again
					for(j=0;j<MAXPL;j++){
						pline[j].ResetScore(); //reset score
					}
					roundcnt=0;
					alivecnt=0;
					quit2=true; //stop waiting for y or n
					break;
				case SDLK_ESCAPE:
					//return to menu
					quit=true;
					quit2=true;
					alivecnt=3; //do not prepare game field when quiting ;-)
					break;
			}
		  }
		}
		//SDL_Delay(5000); //wait 5s before exit
		//quit=true;
	}
	if(alivecnt<2){		//less then 2 players can't play
		//start new round
		SDL_Delay(1000); //wait 1s betwean rounds
		for(j=0;j<MAXPL;j++){			
			if(pline[j].lkey!=SDLK_CLEAR || pline[j].mouse){ 		//if player is active
				pline[j].Reset(); 		//reset him
			}
		}
		alivecnt=oldalivecnt;		//all players are alive again
		if(!(strcmp(bgpath,"none"))){
			SDL_FillRect(screen,NULL,0);	//clear the screen
			SDL_Flip(screen);
		}else{
			DisplayImage(bgimage,screen,0,0);
		}
		//draw border that players must not hit
		SDL_drawLine(screen, 5,      5,      xres-5, 5,      borderclr);
                SDL_drawLine(screen, xres-5, 5,      xres-5, yres-5, borderclr);
                SDL_drawLine(screen, xres-5, yres-5, 5,      yres-5, borderclr);
                SDL_drawLine(screen, 5,      yres-5, 5,      5,      borderclr);
                SDL_drawLine(screen, xres-40,5, xres-40, yres-5, borderclr);
		SDL_Flip(screen);												
		if(barriers)
			PlaceBarriers(screen,barcount);
		DisplayScore(screen);
		quit2=false;
		
	}
       SDL_Delay(400/speed);
      }
      }
      for(j=0;j<MAXPL;j++){
	      pline[j].dead=true; //leaving the game loop, all lines must be dead
	      pline[j].mouse=false; //none of them has set mouse flag
      }
}

bool menu(SDL_Surface *screen){
	//variables
	SDL_Event event;
	bool quit=false;
	bool lf=true; //set left or right key?
	int lineno=0; //number of edited line, 0 = no line edited
	SDL_Rect tmprect;
	Uint32 flags;
	SDL_Surface *tmpsurf;
	SDL_Color barcolor;
	char tmptxt[30];
	char tmpno[5];
	DIR *dir;
	struct dirent *dent;

	barcolor.r=236;
	barcolor.g=133;
	barcolor.b=0;

	flags=screen->flags;
	screen=SDL_SetVideoMode(XRES,YRES,16,flags); //changing to default resolution - because of menu image
	
	xres=XRES;
	yres=YRES;

	speed=5;

	barriers=false;
	DispMenu(screen);	//display menu
	DispText(screen,barcolor,100,320,125,20,"(B) Barriers: off");
	strcpy(tmptxt,"(R) Resolution: ");
	sprintf(tmpno,"%d",xres);
	strcat(tmptxt,tmpno);
	strcat(tmptxt,"x");
	sprintf(tmpno,"%d",yres);
	strcat(tmptxt,tmpno);
	DispText(screen,barcolor,100,370,220,20,tmptxt);
	strcpy(tmptxt,"(C) Barriers count: ");
	sprintf(tmpno,"%d",barcount);
	strcat(tmptxt,tmpno);
	DispText(screen,barcolor,100,345,220,20,tmptxt);
	strcpy(bgpath,"none");
	DispText(screen,barcolor,400,320,240,20,"(M) Map: none");
	DispText(screen,barcolor,400,345,220,20,"(S) Speed: 5");

	dir=opendir("maps"); //open maps directory
	while(!quit){
		SDL_Delay(1);	//don't use 100% cpu	
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_MOUSEBUTTONDOWN:
					if(lineno!=0){
						pline[lineno-1].mouse=true; //this line is controlled by mouse
						DispText(screen,pline[lineno-1].GetColor(),240,((lineno-1)*30+100),250,30,"mouse left and right button");
						pline[lineno-1].Reset();
						pline[lineno-1].ResetScore();
						lineno=0;		//don't set up by keyboard
						
					}
					break;
				case SDL_KEYDOWN:
				     if(lineno==0){ 		//if I am not catching keys for some line
					switch(event.key.keysym.sym){	//I am catching control keys
						case SDLK_SPACE:
							//start the game
							return true;
							break;
						case SDLK_ESCAPE:
							//exit program
							quit=true;
							break;
						case SDLK_f:							
#ifdef WIN32
							flags=screen->flags;
							flags ^= SDL_FULLSCREEN;	//toggle fulscreen flag
							screen=SDL_SetVideoMode(XRES,YRES,16,flags); //recreate window
							DispMenu(screen);
							DispText(screen,barcolor,100,320,125,20,"(B) Barriers: off");
                          				strcpy(tmptxt,"(R) Resolution: ");
                      				     	sprintf(tmpno,"%d",xres);
                 			          	strcat(tmptxt,tmpno);
                        			   	strcat(tmptxt,"x");
                 			          	sprintf(tmpno,"%d",yres);
           			                	strcat(tmptxt,tmpno);
          			                 	DispText(screen,barcolor,100,370,220,20,tmptxt);
                   			        	strcpy(tmptxt,"(C) Barriers count: ");
    			                	      	sprintf(tmpno,"%d",barcount);
                        			  	strcat(tmptxt,tmpno);
     				                     	DispText(screen,barcolor,100,345,220,20,tmptxt);
             				             	strcpy(tmptxt,"(M) Map: ");
                        			  	strcat(tmptxt,bgpath);
                       				   	DispText(screen,barcolor,400,320,240,20,tmptxt);
						
#else							
							SDL_WM_ToggleFullScreen(screen);
#endif
							break;
						case SDLK_b: //toggle barriers on/off
							barriers=!barriers;
							strcpy(tmptxt,"(B) Barriers: ");
							barriers?strcat(tmptxt,"on"):strcat(tmptxt,"off");
							DispText(screen,barcolor,100,320,160,30,tmptxt);
							break;
						case SDLK_r: //change resolution
							xres=(int) xres*125/100;
							xres==1000?xres=1024:xres=xres;
							yres=(int) xres*75/100;
							yres==960?yres=1024:yres=yres;
							if(xres>1280){
								xres=640;
								yres=480;
							}
							strcpy(tmptxt,"(R) Resolution: ");
							sprintf(tmpno,"%d",xres);
							strcat(tmptxt,tmpno);
							sprintf(tmpno,"%d",yres);
							strcat(tmptxt,"x");
							strcat(tmptxt,tmpno);
							DispText(screen,barcolor,100,370,260,23,tmptxt);
							break;
						case SDLK_c:
							barcount +=2;
							if(barcount>40)
								barcount=6;

							strcpy(tmptxt,"(C) Barriers count: ");
							sprintf(tmpno,"%d",barcount);
							strcat(tmptxt,tmpno);
							DispText(screen,barcolor,100,345,220,23,tmptxt);
							break;
						case SDLK_m:
							if(NULL == (dent=readdir(dir))) { //if I am at the end of directory list
								rewinddir(dir); //return to beginning
								strcpy(bgpath,"none"); //in the end of list is none background
							}else{
								strcpy(bgpath, dent->d_name); //copy directory name to bgpath
								if(!(strcmp(bgpath,"."))) { //skip . entry
									// seekdir(dir,(telldir(dir)+1)); //skip following .. entry
									if (NULL == (dent=readdir(dir))) { //end-of-directory routine
										rewinddir(dir);
										strcpy(bgpath,"none");
									} else {
										strcpy(bgpath,dent->d_name);
									}
								}
								if(!(strcmp(bgpath,".."))) { //skip . entry
									// seekdir(dir,(telldir(dir)+1)); //skip following .. entry
									if (NULL == (dent=readdir(dir))) { //end-of-directory routine
										rewinddir(dir);
										strcpy(bgpath,"none");
									} else {
										strcpy(bgpath,dent->d_name);
									}
								}
							}
							//display what did I get
							strcpy(tmptxt,"(M) Map: ");
							strcat(tmptxt,bgpath);
							DispText(screen,barcolor,400,320,240,30,tmptxt);
							break;
						case SDLK_s:
							speed++;
							if(speed>10)	//speed is 1 to 10
								speed=1;
							sprintf(tmptxt,"%d",speed); //convert int to char
							DispText(screen,barcolor,503,345,220,30,tmptxt);
							break;
						case SDLK_F1:
							//the first line setup
							lineno=1;
							break;
						case SDLK_F2:
							//second...
							lineno=2;
							break;
						case SDLK_F3:
							lineno=3;
							break;
						case SDLK_F4:
							lineno=4;
							break;
						case SDLK_F5:
							lineno=5;
							break;
						case SDLK_F6:
							lineno=6;
							break;
							
					}
					lf=true; //first set left key
				   }else{
					if(lf){	
				       		pline[lineno-1].lkey=event.key.keysym.sym; //set the left key of current line object
						DispText(screen,pline[lineno-1].GetColor(),240,((lineno-1)*30+100),270,30,SDL_GetKeyName(pline[lineno-1].lkey));
						 lf=false; //done setting left key
					}else{
						pline[lineno-1].rkey=event.key.keysym.sym;
						DispText(screen,pline[lineno-1].GetColor(),380,((lineno-1)*30+100),0,0,SDL_GetKeyName(pline[lineno-1].rkey));
						
						pline[lineno-1].Reset(); 	//activate the line
						pline[lineno-1].ResetScore(); 	//zero player's score
						lineno=0; 			//setup done
					}
						
					      
				   }
			}
		}
	
	}
	closedir(dir);
	return false; //do not exit program
	
}


SDL_Surface *LoadImage(char* file){
        SDL_Surface *image;
        image=IMG_Load(file);   
        if (image==NULL){  
		fprintf(stderr,"Loading image %s failed\n",file);		//on non Win32 just print message to stderr
                return (0);
        }
        return(image);                                                          }
					
void DisplayImage(SDL_Surface *image,SDL_Surface *screen,int x,int y){
        SDL_Rect dest;
        dest.x=x;
        dest.y=y;
        SDL_BlitSurface(image,NULL,screen,&dest);  
        SDL_UpdateRects(screen,1,&dest); //refresh
}                                       


line::line(){ //initialize some variables
	x=0;y=0;
	left=false;	
	right=false;
	dead=true;	//all lines born dead 
	score=0;	//with zero score
	mouse=false;	//default controlled by keyboard
}
void line::ChangeDirection(bool plus){
	if(plus){
		direction++;
		if(direction>19)
			direction=0; //point 20 = point 0
	}else{
		direction--;
		if (direction<0)
			direction=19; //point -1=point 19
	}
}
void line::Move(bool space){
	int nx,ny;
	Uint32 tmpcolor; 	//color of line in Unint32 format
	int i;
	Uint32 pxcolor;		//color of pixel where line is
	SDL_Rect tmprect;
	
	if(lkey!=SDLK_CLEAR || mouse){
		tmpcolor=SDL_MapRGB(dest->format,color.r,color.g,color.b); //change color format
		nx=x+cx[direction]; //change direction using coeficint table
		ny=y+cy[direction];
		if(space){	//make the space
			x=nx;
			y=ny;
			nx=x+cx[direction];
			ny=y+cy[direction];
			x=nx;y=ny;
			nx=x+cx[direction];
			ny=y+cy[direction];
		}
		pxcolor=GetPixelColor(dest,nx,ny); 	//get color of pixel, where I'm going
		pxcolor +=GetPixelColor(dest,nx+1,ny+1);
		pxcolor +=GetPixelColor(dest,nx+1,ny);

		pxcolor +=GetPixelColor(dest,nx-1,ny-1);
		pxcolor +=GetPixelColor(dest,nx-1,ny);
		
		pxcolor +=GetPixelColor(dest,nx,ny+1);
		pxcolor += GetPixelColor(dest,nx,ny-1); 
		if(pxcolor!=0){ 			//if line is on non-black pixel, it hit other line
			dead=true; 			//so it's dead
			alivecnt--;			//and we have one alive line less
			for(i=0;i<MAXPL;i++){
				if(!pline[i].dead)  	//if player isn't dead
					pline[i].IncScore(); //increase him score
			}
			DisplayScore(dest);
		}
		if(nx<=5 || ny<=5 || nx>=(xres-40) || ny>=(yres-5)){         //if line reaches border of screen
			dead=true;                              //it dies
			alivecnt--;                             //and we have one alive line less
			for(i=0;i<MAXPL;i++){
				if(!pline[i].dead)              //if player isn't dead
					pline[i].IncScore();    //increase him score
			}
			DisplayScore(dest);
			return;
		}
				
		  SDL_drawLine_AA(dest,x,y,nx,ny,tmpcolor);
		  if((x-nx)>0){		//moving left
		  	tmprect.x=nx;  //nx is left corner of updated rect
		  }else{
			tmprect.x=x;
		  }
		  if((y-ny)>0){		//moving top
			  tmprect.y=ny; //ny is top corner of updated rect
		  }else{
			  tmprect.y=y;
		  }
		  tmprect.w=6;
		  tmprect.h=6;
		  if((tmprect.x+10)<=xres && (tmprect.y+10)<=yres) //cannot update out of surface
		  	SDL_UpdateRects(dest,1,&tmprect);
	
		x=nx;
		y=ny;
	}
}

void line::SetColor(int r,int g,int b){
	color.r=r;
	color.g=g;
	color.b=b;
	color.unused=0;
}
void line::Reset(){
	left=false;
	right=false;
	dead=false;
	direction=irand(19);	//random coeficient of direction
	x=irand(xres-200) + 100;		//random position
	y=irand(yres-100) + 100;

}

Uint8 GetPixelColor (SDL_Surface *surface,int x,int y){
    Uint8 *pixel;
    pixel= (Uint8 *) ((Uint8*)surface->pixels + (surface->pitch * y)+ (surface->format->BytesPerPixel * x));
    return (*pixel); 
}

int irand(int max){		//function for random integer
	int tmp;
	tmp=(int) (rand()/(((double)RAND_MAX+(double)1)/(double)max));
	if(tmp<0)
		tmp *=-1;	
	return tmp;
}

void DisplayScore(SDL_Surface *screen){
	int i;
	char tmp[3];
	SDL_Rect tmprect;

	for(i=0;i<MAXPL;i++){
		if(pline[i].lkey!=SDLK_CLEAR || pline[i].mouse){ //if line is active
			sprintf(tmp,"%d",pline[i].GetScore()); //convert score to char
			DispText(screen,pline[i].GetColor(),(xres-30),(i*30+100),24,28,tmp);
		}
	}
}

char* GetAppPath(){
	char *wd;
	wd= new char[100];
#ifdef WIN32
#define MAX_STRING_SIZE 100
	//on windows use winapi function to get it
		char *tmp2,*tmp3;

	    //get the instance of the application


	     
	    //with the instance finf the full path and name of the application
	    char file[MAX_STRING_SIZE];
	    GetModuleFileName(hInstance, file, MAX_STRING_SIZE);
    	tmp2=file;
	    tmp3=strrchr(file,'\\'); //find the last backslash
        *tmp3='\0'; //tarminating character replaces last slash of path
	    sprintf(wd,"%s",tmp2); //copy path on the other place in memory	       


#else
	//on linux read it form /proc/self/maps
	FILE *f;
	char tmp[200];
	char *tmp2,*tmp3;
	f=fopen("/proc/self/maps","r"); //that file contains path to my aplication
	fgets(tmp,200,f); //read the first line
	fclose (f);
	tmp[strlen(tmp)-1]='\0'; //remove newline character
	tmp2=strchr(tmp,'/'); //find first slash
	tmp3=strrchr(tmp,'/'); //find the last slash
	*tmp3='\0'; //tarminating character replaces last slash of path
	sprintf(wd,"%s",tmp2); //copy path on the other place in memory

#endif
	return (wd);
}
#ifdef WIN32
void StoreHinstance(HINSTANCE hi){
     hInstance=hi;
}
#endif
void DispMenu(SDL_Surface *screen){
		char tmp[20];
		char tmp2[2];
		int i;

		DisplayImage(bg,screen,0,0); //display background
		for(i=0;i<MAXPL;i++){ //display F texts
		        sprintf(tmp2,"%d",i+1);
			strcpy(tmp,"F");
			strcat(tmp,tmp2);
			DispText(screen,pline[i].GetColor(),100,(i*30+100),0,0,tmp);
			pline[i].lkey=SDLK_CLEAR; //disable all lines - return to default state
			pline[i].dead=true; //all lines are dead by default
		}
}

void PlaceBarriers(SDL_Surface *screen, int count){
	
	int i,x,y;
	SDL_Surface *image = new SDL_Surface;
	image = LoadImage("barrier.jpg");
	for(i=0; i<=count; i++){
		x=irand(xres);
		y=irand(yres);
		DisplayImage(image,screen,x,y);
	}
	SDL_FreeSurface(image);
}
void DispText(SDL_Surface *screen,SDL_Color color,int x,int y,int w, int h, char *text){
	SDL_Rect dest;
	dest.x=x;
	dest.y=y;
	dest.w=w;
	dest.h=h;
	SDL_FillRect(screen,&dest,0);
	SDL_UpdateRects(screen,1,&dest);
	DisplayImage(TTF_RenderText_Blended(font,text,color),screen,x,y);
}
