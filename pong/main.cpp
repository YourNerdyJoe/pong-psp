#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#define SCREENWIDTH 480
#define SCREENHEIGHT 272
#define B_RIGHT 9
#define B_LEFT 7
#define B_UP 8
#define B_DOWN 6
#define B_L 4
#define B_R 5
#define B_CROSS 2
#define B_CIRCLE 1
#define B_SQUARE 3
#define B_TRIANGLE 0
#define B_START 11
#define B_SELECT 10

extern "C" int main(int argc,char* argv[])
{
	Uint32 t0,t1;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
	TTF_Init();
	TTF_Font* font = NULL;
	TTF_Font* font2 = NULL;
	font = TTF_OpenFont("font/Fixedsys500c.ttf",24);
	font2 = TTF_OpenFont("font/Fixedsys500c.ttf",10);
	SDL_Surface* score1 = NULL;
	SDL_Surface* score2 = NULL;
	SDL_Color color = {255,255,255};	//font color
	SDL_Rect score1offset,score2offset;
	score1offset.x = SCREENWIDTH/2-32;
	score1offset.y = 3;
	score2offset.x = SCREENWIDTH/2+8;
	score2offset.y = 3;
	char tmp[32];
	SDL_Joystick* joystick = NULL;
	if(SDL_NumJoysticks())
	{
		joystick = SDL_JoystickOpen(0);
	}
	bool autopilot = false;
	bool quit = false;
	SDL_Event event;
	SDL_Rect player,enemy,ball;
	player.w = enemy.w = 16;
	player.h = enemy.h = 64;
	player.x = 32;
	player.y = (SCREENHEIGHT/2)-(player.h/2);
	enemy.x = SCREENWIDTH-32-enemy.w;
	enemy.y = (SCREENHEIGHT/2)-(enemy.h/2);
	ball.x = 90;
	ball.y = 90;
	ball.w = ball.h = 16;
	int ballxspd=-2,ballyspd=3;
	int enemyscore=0,playerscore=0;
	while(!quit)
	{
		t0 = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_JOYBUTTONDOWN:
				if(event.jbutton.button==B_START)
				{
					if(autopilot) autopilot=false;
					else autopilot=true;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			default:
				break;
			}
		}
		//-------------- UPDATE --------------//
		//move player
		if(autopilot)
		{
			if(ballxspd<0)
			{
				if(ball.y<player.y)
				{
					player.y-=5;
				}
				else if(ball.y>player.y+player.h)
				{
					player.y+=5;
				}
			}
		}
		else
		{
			if(SDL_JoystickGetButton(joystick,B_UP))
			{
				player.y-=5;
			}
			if(SDL_JoystickGetButton(joystick,B_DOWN))
			{
				player.y+=5;
			}
		}
		if(player.y<=0) player.y = 0;
		else if(player.y+player.h>=SCREENHEIGHT) player.y = SCREENHEIGHT-player.h;
		//move ball
		ball.x += ballxspd;
		ball.y += ballyspd;
		if(ball.y<=0)
		{
			ballyspd = -ballyspd;
			ball.y = 0;
		}
		else if(ball.y+ball.h>=SCREENHEIGHT)
		{
			ballyspd = -ballyspd;
			ball.y = SCREENHEIGHT-ball.h;
		}
		if(ball.x<=player.x+player.w && ball.x+(ball.w/4)>=player.x && ball.y<player.y+player.h && ball.y+ball.h>player.y)
		{
			if(ballxspd<0)
				ballxspd = -ballxspd;
			ballyspd = -((player.y+(player.h/2)-ball.y)/5);
		}
		if(ball.x+ball.w>=enemy.x && ball.x<=enemy.x+(ball.w/4) && ball.y<enemy.y+enemy.h && ball.y+ball.h>enemy.y)
		{
			if(ballxspd>0)
				ballxspd = -ballxspd;
			ballyspd = -((enemy.y+(enemy.h/2)-ball.y)/5);
		}
		//move enemy
		if(ballxspd>0)
		{
			if(ball.y<enemy.y)
			{
				enemy.y-=5;
			}
			else if(ball.y>enemy.y+enemy.h)
			{
				enemy.y+=5;
			}
		}
		//check if scored
		if(ball.x<=0)
		{
			enemyscore++;
			ball.x = 90;
			ball.y = 90;
			ballxspd=-2,ballyspd=3;
		}
		else if(ball.x+ball.w>=SCREENWIDTH)
		{
			playerscore++;
			ball.x = 90;
			ball.y = 90;
			ballxspd=-2,ballyspd=3;
		}
		//------------------------------------//

		//-------------- RENDER --------------//
		//clear screen
		SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
		//draw player
		SDL_FillRect(screen,&player,SDL_MapRGB(screen->format,255,255,255));
		//draw enemy
		SDL_FillRect(screen,&enemy,SDL_MapRGB(screen->format,255,255,255));
		//draw ball
		SDL_FillRect(screen,&ball,SDL_MapRGB(screen->format,255,255,255));
		//draw player score
		sprintf(tmp,"%d",playerscore);
		score1 = TTF_RenderText_Solid(font,tmp,color);
		SDL_BlitSurface(score1,NULL,screen,&score1offset);
		SDL_FreeSurface(score1);
		//draw enemy score
		sprintf(tmp,"%d",enemyscore);
		score2 = TTF_RenderText_Solid(font,tmp,color);
		SDL_BlitSurface(score2,NULL,screen,&score2offset);
		SDL_FreeSurface(score2);

		if(autopilot)
		{
			score1 = TTF_RenderText_Solid(font2,"autopilot",color);
			SDL_BlitSurface(score1,NULL,screen,NULL);
			SDL_FreeSurface(score1);
		}

		SDL_Flip(screen);
		//------------------------------------//
		while(1)
		{
			t1=SDL_GetTicks();
			if(t1-t0>=1000/60)	// 1000/60 = 60fps
				break;
		}
	}
	if(joystick)
	{
		SDL_JoystickClose(joystick);
	}
	TTF_CloseFont(font);
	TTF_CloseFont(font2);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
