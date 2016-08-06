#include <iostream>
#include "Game.h"

using namespace std;

void printhelp(bool errorhelp)
{
	cout<<endl;
	if(!errorhelp)
	{
		cout<<"A Game based on ncurses. Play by clicking the tiles with the mouse"<<endl;
		cout<<"Further instructions by pressing 'h' inside the game"<<endl<<endl;
	}

	cout
	<<"Usage: "													<<endl
	<<"numberscratcher [flags]"									<<endl
																<<endl
	<<"Accepted flags:"											<<endl
	<<"-h: prints this help menu"								<<endl
	<<"-?: same as -h"											<<endl
	<<"-g [Number]: loads the Game specified by the number."	<<endl
	<<"-s  [file] saves the gamestate in [file] on exit"		<<endl
	<<"-l  [file] loads the gamestate in [file]"				<<endl
	<<"-ls [file] combination of the two commands above"		<<endl
																<<endl;
	exit(0);
}

int main(int argc, char** argv)
{
	int presetgame=1;
	string loadfilename;
	string savefilename;
	for(int i=0;i<argc;++i)
	{
		if(argv[i][0]=='-')
		{
			switch(argv[i][1])
			{
				case 'h':
				case '?':
					printhelp(false);
				case 'g':
					if(++i < argc && argv[i][0]!='-')
						presetgame = atoi(argv[i]);
					else
						printhelp(true);
					break;
				case 's':
					if(++i < argc && argv[i][0]!='-')
						savefilename = argv[i];
					else
						printhelp(true);
					break;
				case 'l':
					if(++i < argc && argv[i][0]!='-')
						loadfilename = argv[i];
					else
						printhelp(true);
					break;
					if(argv[i-1][2]=='s')
						savefilename=loadfilename;
			}
		}
	}
	if (loadfilename.empty())
	{
		loadfilename = "fields/";
		loadfilename += (presetgame+'0');
	}
	std::cout<<"Loading File: "<<loadfilename<<std::endl;
	Game::startGame(loadfilename,savefilename);
	return 0;
}
