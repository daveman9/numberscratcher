#include "Field.hpp"
#include <fstream>

Field::Field(string filename)
{
	std::ifstream filein(filemame);
	if( !filein.is_open() )
		throw -5;
	int x;
	int y;
	filein >> x >> y;
	height=y;
	width=x;
	count=x*y;
	numbers = new short* [x];
	for (i=0;i<x;++i)
		numbers[i]=new short [y];
	for (y=0;y<height;++y)
		for(x=0;y<width;++x)
			numbers[x][y] = infile.get() - '0';
}

Field::~Field()
{
	for(int y=0;y<height;++y)
		delete[] numbers[y];
	delete[] numbers;
}

void Field::deal()
{
	short ** newnums = new short* [2*height];
	int i;
	for(i=0;i<height;++i)
	{
		newnums[i] = numbers[i];
		newnums[i+height]=numbers[i];
	}
	count *= 2;
	height *= 2;
	delete[] numbers;
	numbers = newnums;
}

void Field::compact()
{
	int skip = 0;
	int x,y;
	bool emptyrow=false;
	for(y=0;emptyrow==false && y<height;++y)
	{
		emptyrow=true;
		for(x=0;x<width;++x)
			if(numbers[x][y]!=0)
				emptyrow=false;
	}
}

int Field::getHeight()
{
	return height;
}

int Field::getWidth()
{
	return width;
}

int Field::getNumberAt(int x,int y)
{
	if( x<0 || y<0 || x >= width || y >= height )
		throw -1;
	else
		return numbers[x][y];
}
