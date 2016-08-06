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


	//Cursor position - In number Coordinates.
	int xpos=0;
	int ypos=0;
	//Dimensions of the numbers!
	int width,height;

	//Selection Status and position
	bool firstselected=false;
	bool secondselected=false;
	int selx1,selx2,sely1,sely2;

	//General Status Variables
	bool gamerunning=true;
	bool movepossible,showmovehint=false;
	bool showmove = false;

	//the hint
	std::string movehint;
	int hintx1,hinty1,hintx2,hinty2;

	//the current view status
	int viewOffset = 0;
	int viewheight;
	//Never used but needed for getmaxyx()
	int screenwidth;

	//if the bool is false returns the number coordinate
	auto translateview = [&](bool toScreenCoordinates, int y){
		if(toScreenCoordinates)
			return ( y - viewOffset);
		else
			return ( y + viewOffset);
	};

	auto printhelp = [&](){
		clear();
		mvprintw(0,0,"Move using the mouse or the arrow keys");
		mvprintw(1,0,"d         : Deal more Numbers");
		mvprintw(2,0,"r         : Reset selection.");
		mvprintw(3,0,"q         : Quit the game ");
		mvprintw(4,0,"s,e,Enter : Select the number under the Cursor");
		mvprintw(5,0,"m         : check whether a move is possible.");
		mvprintw(6,0,"Page Down : Scroll screen down by 5 lines");
		mvprintw(7,0,"Page Up   : Scroll screen up by 5 lines");
		mvprintw(9,0,"Press any Key to resume");
		getch();
	};

	auto printNumbers = [&](){
		getmaxyx(stdscr, viewheight, screenwidth);
		clear();
		short n;
		int x,y;
		width = f.getWidth();
		height= f.getHeight();
		for(y=translateview(false,0);y<height && y < translateview(false,viewheight-3);++y)
		{
			move( translateview(true,y), 0);
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
			/* Prints the line number to the right side
			printw("     ");
			printw(std::to_string(y).c_str());
			*/
		}
		y = translateview(true,y);
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
					if(f.getHeight() == ypos)
						--ypos;
					firstselected=false;
					if(f.getCount()==0)
					{
						clear();
						mvprintw(0,0,"You won!");
						mvprintw(2,0,"Press any key to end");
						getch();
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
			if(ypos < viewOffset)
				viewOffset = ypos;
			if(ypos > translateview(false,viewheight-4))
				++viewOffset;
		}
	};

	//if up is false scrolls down
	auto scrollaction = [&](bool up){
		if(up)
		{
			viewOffset -= 5;
			if (viewOffset < 0)
				viewOffset = 0;
			if ( ypos > (viewOffset + viewheight - 4) )
				ypos = viewOffset + viewheight - 4;
		}
		else if( height > (viewheight -3) )
		{
			viewOffset += 5;
			int maxoffset = height - (viewheight - 3);
			if( maxoffset > 0 && viewOffset > maxoffset)
				viewOffset = maxoffset;
			if( translateview(true,ypos) < 0)
				ypos = translateview(false,0);
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
			case 'z':
				++viewOffset;
				break;
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
			case KEY_PPAGE:
				scrollaction(true);
				break;
			case KEY_NPAGE:
				scrollaction(false);
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
				endwin();
				f.deal();
				initscr();
				width=f.getWidth();
				height=f.getHeight();
				break;
			case KEY_MOUSE:
				if(getmouse(&event)==OK)
				{
					if(event.bstate & BUTTON1_CLICKED)
					{
						int mx = event.x/2;
						int my = translateview(false, event.y);
						if(mx>=0 && my >=0 && mx<(2*width-1) && my<height)
						{
							xpos = event.x/2;
							ypos = my;
							selectAction();
						}
					}
					if(event.bstate & BUTTON4_PRESSED)
						scrollaction(true);
					if(event.bstate & BUTTON5_PRESSED)
						scrollaction(false);
				}
				break;
			case 's':
			case 'e':
			case 10:
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
