#include "Field.hpp"
#include <fstream>
#include <iostream>

Field::Field(std::string& filename)
{
	std::ifstream filein(filename);
	if( !filein.is_open() )
		throw -5;
	int x;
	int y;
	filein >> x;
	filein >> y;
	height=y;
	width=x;
	count=x*y;
	//std::cout<<x << std::endl << y << std::endl;
	numbers = new short* [y];
	short n;
	filein.get();
	for (int i=0;i<y;++i)
		numbers[i]=new short [x];
	for ( y=0; y < height; ++y)
	{
		for(x=0; x<width; ++x)
		{
			n = filein.get() - '0';
		//	std::cout << n;
			numbers[y][x] = n;
		}
	}
	filein.close();
//	std::cout << std::endl;
}

Field::~Field()
{
	for(int i = 0; i < reservedheight; ++i)
		delete[] numbers[i];
	delete[] numbers;
}

void Field::deal()
{
	reservedheight = 2* height;
	short ** newnums = new short * [reservedheight];
	int endx=width-1;
	int endy=height-1;
	//Find the position to append the new numbers to
	while(numbers[endy][endx]==0)
	{
		if(--endx < 0)
		{
			endx=width-1;
			--endy;
		}
	}
	//Set the position where the new numbers will be written
	if(++endx==width)
	{
		endx = 0;
		++endy;
	}
	int wx=endx;
	int wy=endy;
	//Copy the old numbers over
	int i;
	for(i=0;i<height;++i)
		newnums[i]=numbers[i];
	//Reserve more empty space if needed
	for(int a;i<reservedheight;++i)
	{
		newnums[i] = new short [width];
		for(a=0;a<width;++a)
			newnums[i][a] = 0;
	}
	//Write the numbers
	int x,y;
	short n;
	for(y=0; y<endy; ++y)
	{
		for(x=0; x<width; ++x)
		{
			n = numbers[y][x];
			if(n != 0)
			{
				newnums[wy][wx] = n;
				if(++wx == width)
				{
					wx = 0;
					++wy;
					++height;
				}
			}
		}
	}
	for(x=0;x<endx;++x)
	{
		n = numbers[endy][x];
		if (n != 0)
		{
			newnums[wy][wx]=n;
			if(++wx == width)
			{
				wx = 0;
				++wy;
				++height;
			}
		}
	}
	delete[] numbers;
	numbers = newnums;
}

void Field::compact()
{
	bool emptyrow;
	short * p;
	int skip = 0;
	for(int x,y = 0;y<height;++y)
	{
		emptyrow=true;
		for(x=0;x<width;++x)
			if(numbers[y][x] != 0)
				emptyrow=false;
		if(skip > 0)
		{
			p = numbers[y-skip];
			numbers[y-skip] = numbers[y];
			numbers[y] = p;
		}
		if(emptyrow)
			++skip;
	}
	height -= skip;
}
void Field::compact(int y1, int y2)
{
	bool y1empty=true;
	bool y2empty=true;
	for(int x=0;x<width;++x)
	{
		if(numbers[y1][x] != 0)
			y1empty=false;
		if(numbers[y2][x] != 0)
			y2empty=false;
	}
	short * p;
	if(y1empty)
	{
		for(int y=y1+1;y<height;++y)
		{
			p = numbers[y-1];
			numbers[y-1] = numbers[y];
			numbers[y] = p;
		}
		--height;
		if(y2empty && y2 > y1)
			--y2;
	}
	if(y2empty)
	{
		for(int y=y2+1;y<height;++y)
		{
			p = numbers[y-1];
			numbers[y-1] = numbers[y];
			numbers[y] = p;
		}
		--height;
	}
}

bool Field::remove(int x1, int y1, int x2, int y2)
{
	if (x1<0 || x2<0 || y1<0 || y2<0 || x1>= width || y1>=height || x2>=width || y2>=height ||
	   (x1==x2 && y1==y2) || numbers[y1][x1]==0 || numbers[y2][x2]==0 ||
	   !(numbers[y1][x1]==numbers[y2][x2] || numbers[y1][x1]+numbers[y2][x2]==10) )
		return false;
	int x,y;
	//Random lambda
	auto deldel = [&](){
		if(x==x2 && y==y2)
		{
			numbers[y1][x1] = 0;
			numbers[y2][x2] = 0;
			compact(y1,y2);
			count -= 2;
			return true;
		}
		else
			return false;
	};

	//check for same row to the left
	x=x1;
	y=y1;
	do{
		if(--x < 0)
		{
			x=width-1;
			--y;
		}
	}while(y>=0 && numbers[y][x]==0);
	if(y<0)
		++y;
	if(deldel())
		return true;

	//check for same row to the right
	x = x1;
	y = y1;
	do{
		if(++x == width)
		{
			x = 0;
			++y;
		}
	}while(y<height && numbers[y][x]==0);
	if(y==height)
		--y;
	if(deldel())
		return true;

	//check for the same column on top
	x = x1;
	y = y1;
	do
		--y;
	while(y>=0 && numbers[y][x] ==0 );
	if(y<0)
		++y;
	if(deldel())
		return true;

	//check for the same column on bottom
	y = y1;
	do
		++y;
	while(y<height && numbers[y][x]==0);
	if(y==height)
		--y;
	if(deldel())
		return true;
	return false;
}

int Field::getHeight()
{
	return height;
}

int Field::getWidth()
{
	return width;
}

int Field::getNumberAt(int x, int y)
{
	if(x<0 || y<0 || x>=width || y>=height)
		return -1;
	return numbers[y][x];
}

int Field::getCount()
{
	return count;
}

void Field::writeField(std::string& filename)
{
	std::ofstream outfile(filename);
	if(!outfile.is_open())
		throw -5;
	outfile<<width<<" "<<height<<" ";
	for(int x,y=0;y<height;++y)
		for(x=0;x<height;++x)
			outfile.put(numbers[y][x]+'0');
	outfile.close();
}