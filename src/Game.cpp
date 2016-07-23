#include "Game.h"
#include "Field.hpp"
#include <ncurses.h>
#include <iostream>

namespace Game
{

void startGame(std::string inputfile, std::string outputfile)
{
	//initialization
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr ,true);
	start_color();
	mousemask(BUTTON1_CLICKED,NULL);
	MEVENT event;

	//Default color
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	//Cursor color
	init_pair(2,COLOR_BLACK,COLOR_WHITE);
	//Selection color
	init_pair(3,COLOR_RED,COLOR_BLACK);

	Field f(inputfile);

	int width = f.getWidth();
	int height= f.getHeight();
	int xpos=0;
	int ypos=0;
	bool firstselected=false;
	bool secondselected=false;
	int selx1,selx2,sely1,sely2;
	bool gamerunning=true;

	auto printNumbers = [&](){
		clear();
		short n;
		int x,y;
		for(y=0;y<height;++y)
		{
			move(y,0);
			for(x=0;x<width;++x)
			{
				if(xpos==x && ypos==y)
					color_set(2,0);
				if( (firstselected && selx1==x && sely1==y) || (secondselected && selx2==x && sely2==y))
					color_set(3,0);
				n = f.getNumberAt(x,y);
				if(n != 0)
					addch(n + '0');
				else{
					color_set(1,0);
					addch(' ');
				}
				color_set(1,0);
				addch(' ');
			}
		}
		mvprintw(y,0,"Move using the mouse or the arrow keys");
		mvprintw(y+1,0,"d: Deal more Numbers");
		mvprintw(y+2,0,"r: reset selection.");
		mvprintw(y+3,0,"q: Quit the game ");
		mvprintw(y+4,0,"s,e: Select the number under the Cursor");
	};

	auto selectAction = [&](){
		if(!firstselected && f.getNumberAt(xpos,ypos)!=0)
		{
			firstselected=true;
			selx1=xpos;
			sely1=ypos;
		}
		else{
			if(selx1==xpos && sely1==ypos)
				firstselected=false;
			else
			{
				if(f.remove(selx1,sely1,xpos,ypos))
				{
					int newheight=f.getHeight();
					if(newheight<height && ypos==height-1)
						--ypos;
					height=newheight;
					firstselected=false;
					if(f.getCount()==0)
					{
						std::cout<<"You won!";
						gamerunning=false;
					}
				}
			}
		}
	};

	/*
	0: right
	1: down
	2: left
	3: up */
	auto moveAction = [&](char direction){
		int newx=xpos;
		int newy=ypos;
		switch(direction)
		{
			case 0:
				do{
					if(++newx == width)
					{
						newx=0;
						++newy;
					}
				}while(newy<height && f.getNumberAt(newx,newy)==0);
				break;
			case 1:
				do{
					++newy;
				}while(newy<height && f.getNumberAt(newx,newy)==0);
				break;
			case 2:
				do{
					if(--newx < 0)
					{
						--newy;
						newx = width-1;
					}
				}while(newy>=0 && f.getNumberAt(newx,newy)==0);
				break;
			case 3:
				do{
					--newy;
				}while(newy<height && f.getNumberAt(newx,newy)==0);
		}
		if(newx>=0 && newx<width && newy>=0 && newy<height)
		{
			xpos=newx;
			ypos=newy;
		}
	};

	int c;
	while (gamerunning)
	{
		printNumbers();
		refresh();
		c=getch();
		switch(c)
		{
			case 'q':
				gamerunning=false;
				break;
			case 'u':
			case KEY_LEFT:
				moveAction(2);
				break;
			case 'a':
			case KEY_RIGHT:
				moveAction(0);
				break;
			case 'v':
			case KEY_UP:
				moveAction(3);
				break;
			case 'i':
			case KEY_DOWN:
				moveAction(1);
				break;
			case 'r':
				firstselected=false;
				break;
			case 'd':
				f.deal();
				width=f.getWidth();
				height=f.getHeight();
				break;
			case 'c':
				f.compact();
				width=f.getWidth();
				height=f.getHeight();
				break;
			case KEY_MOUSE:
				if(getmouse(&event)==OK)
				{
					if(event.bstate & BUTTON1_CLICKED)
					{
						int mx = event.x/2;
						int my = event.y;
						if(mx>=0 && my >=0 && mx<(2*width-1) && my<height)
						{
							xpos = event.x/2;
							ypos = event.y;
							selectAction();
						}
					}
				}
				break;
			case 's':
			case 'e':
			case KEY_ENTER:
				selectAction();
		}
	}
	curs_set(1);
	endwin();
}

}
