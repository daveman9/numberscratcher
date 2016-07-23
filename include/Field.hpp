#pragma once
#include <string>

class Field
{
public:
	Field(std::string filename);
	~Field();
	//Adds new numbers to the field
	void deal();
	//Removes empty rows. Firs variant checks entire field, second Variant only checks
	//the given rows
	void compact();
	void compact(int y1, int y2);
	//Deletes the two numbers if the conditions are met.
	//Calls compact() afterwards
	void remove(int x1,int y1,int x2, int y2);

	int getHeight();
	int getWidth();
	int getNumberAt(int x,int y);
	int getCount();
private:
	int height;
	int width;
	int count;
	//First parameter: y
	//secomd parameter: x
	short ** numbers;
};