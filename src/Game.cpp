#include "Game.h"
#include "Field.hpp"
#include <curses.h>
#include <iostream>

namespace Game
{

void startGame(std::string inputfile, std::string outputfile)
{
	//initialization
	Field f(inputfile);
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


	int width = f.getWidth();
	int height= f.getHeight();
	int xpos=0;
	int ypos=0;
	bool firstselected=false;
	bool secondselected=false;
	int selx1,selx2,sely1,sely2;
	bool gamerunning=true;
	bool movepossible,showmovehint=false;
	bool showmove = false;
	std::string movehint;
	int hintx1,hinty1,hintx2,hinty2;

	auto printhelp = [&](){
		clear();
		mvprintw(0,0,"Move using the mouse or the arrow keys");
		mvprintw(1,0,"d: Deal more Numbers");
		mvprintw(2,0,"r: reset selection.");
		mvprintw(3,0,"q: Quit the game ");
		mvprintw(4,0,"s,e: Select the number under the Cursor");
		mvprintw(5,0,"m: check whether a move is possible.");
		mvprintw(6,0,"Press any Key to resume");
		getch();
	};

	auto printNumbers = [&](){
		clear();
		short n;
		int x,y;
		for(y=0;y<height;++y)
		{
			move(y,0);
			for(x=0;x<width;++x)
			{
				color_set(1,0);
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
		mvprintw(y,0,"press h for help");
		if(showmovehint)
		{
			if(movepossible)
			{
				mvprintw(y+1,0,"Another move is possible");
				if(showmove)
				{
					mvprintw(y+2,0, movehint.c_str());
				}
			}
			else
				mvprintw(y+1,0,"No further move is possible");
		}
		
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
					showmovehint=false;
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

	printhelp();

	int c;
	while (gamerunning)
	{
		printNumbers();
		refresh();
		c=getch();
		switch(c)
		{
			case 'h':
				printhelp();
				break;
			case 'm':
				if(showmovehint && movepossible)
				{
					if(showmove)
					{
						firstselected=false;
						xpos = hintx1;
						ypos = hinty1;
						selectAction();
						xpos = hintx2;
						ypos = hinty2;
						selectAction();
					}
					else{
						f.generatemove(hintx1,hinty1,hintx2,hinty2);
						movehint = "The move is: "+std::to_string(hintx1+1)+","+std::to_string(hinty1+1)+" "+std::to_string(hintx2+1)+","+std::to_string(hinty2+1);
						showmove = true;
					}
				}
				else
				{
					showmove = false;
					movepossible=f.checkformove();
					showmovehint=true;
				}
				break;
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
				xpos = 0;
				ypos = 0;
				moveAction(0);
				firstselected=false;
				break;
			case 'd':
				if(f.checkformove()==true)
				{
					clear();
					mvprintw(0,0,"Another move is possible.");
					mvprintw(1,0,"press d to deal");
					mvprintw(2,0,"Any other Key aborts");
					c = getch();
					if(c != 'd')
						break;
				}
				showmovehint=false;
				f.deal();
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
	if(!outputfile.empty())
		f.writeField(outputfile);
}

}
