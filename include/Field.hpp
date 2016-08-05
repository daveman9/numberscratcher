#pragma once
#include <string>

class Field
{
public:
	Field(std::string& filename);
	~Field();
	//Adds new numbers to the field
	void deal();
	//Removes empty rows.
	//Only checks the given rows
	void compact(int y1, int y2);
	//Deletes the two numbers if the conditions are met.
	//Calls compact() afterwards
	//returns true if something was deleted, false otherwise
	bool remove(int x1,int y1,int x2, int y2);

	//returns true if another move is possible
	//false otherwise
	bool checkformove();

	//Generates a possible move
	//sets x1 to -1 if no move exists
	void generatemove(int& x1, int& y1, int& x2, int& y2);

	int getHeight();
	int getWidth();
	int getNumberAt(int x,int y);
	int getCount();

	void writeField(std::string& filename);
private:
	// Wrapper Function
	// Calls the functions below
	// 0: left
	// 1: up
	// 2: right
	// 3: down
	void next(char direction, int x, int y, int& newx, int& newy);

	void nextleft(int x,int y,int& newx,int& newy);
	void nextright(int x,int y,int& newx,int& newy);
	void nextup(int x,int y,int& newx, int& newy);
	void nextdown(int x,int y,int& newx, int& newy);

	int height;
	int width;
	int count;
	//First parameter: y
	//secomd parameter: x
	int ** numbers;
};
