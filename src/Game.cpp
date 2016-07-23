#include "Game.h"
#include "Field.hpp"
#include <ncurses.h>
#include <iostream>
namespace Game
{

void startGame(std::string inputfile, std::string outputfile)
{
	Field f(inputfile);
	initscr();
	cbreak();
	noecho();
	keypad(stdscr ,true);
	
	int width = f.getWidth();
	int height= f.getHeight();
	for(int x,y=0;y<height;++y)
	{
		for(x=0;x<width;++x)
		{
			addch(f.getNumberAt(x,y)+'0');
		}
		move(y+1,0);
	}
	refresh();
	getch();
	endwin();
}

}
