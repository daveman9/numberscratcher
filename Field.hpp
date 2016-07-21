#pragma once
include <string>

class Field
{
public:
	Field(string filename);
	~Field();
	void deal();
	void compact();
	bool remove(int x1,int y1,int x2, int y2);
	int getHeight();
	int getWidth();
	int getNumberAt(int x,int y);
private:
	int height;
	int width;
	int count;
	short ** numbers;
}
