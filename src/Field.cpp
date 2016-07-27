#include "Field.hpp"
#include <fstream>
#include <iostream>

Field::Field(std::string& filename)
{
	std::ifstream filein(filename);
	if( !filein.is_open() )
		throw -5;

	filein >> width;
	filein >> height;

	count=0;
	numbers = new char* [height];

	//Skip seperator betwenn numbers and Metadata
	filein.get();

	//Reserve Space in the rows
	for (int i=0;i<height;++i)
		numbers[i]=new char [width];

	//Fill the field with the numbers read from the file
	char n;
	for ( int x,y=0; y<height; ++y)
	{
		for(x=0; x<width; ++x)
		{
			n = filein.get() - '0';
			if(n!=0)
				++count;
			numbers[y][x] = n;
		}
	}

	filein.close();
}

Field::~Field()
{
	//Any rows beyond height have been deleted the Moment they became empty
	for(int i = 0; i < height; ++i)
		delete[] numbers[i];
	delete[] numbers;
}

void Field::deal()
{
	int newheight = height + (count / width) + ((count % width == 0)? 0 : 1);
	char ** newnums = new char* [newheight+1];

	//move the old numbers over
	for(int i=0; i<height; ++i)
		newnums[i] = numbers[i];

	for(int i=height;i<=newheight;++i)
		newnums[i] = new char[width];

	//find the correct position
	int wx=width-1;
	int wy=height-1;
	while(numbers[wy][wx]==0)
		--wx;

	if(++wx==width)
	{
		wx = 0;
		++wy;
	}

	//delimiters
	int ry=wy;
	int rx=wx;


	//copying loop
	for(int x,y=0;y<=ry;++y)
	{
		for(x=0;(y<ry && x<width) || (y==ry && x<rx); ++x)
		{
			if(numbers[y][x] != 0)
			{
				newnums[wy][wx] = numbers[y][x];
				if(++wx==width)
				{
					wx = 0;
					++wy;
				}
			}
		}
	}

	//setting the new numbers
	count *= 2;
	height = newheight;
	delete[] numbers;
	numbers = newnums;

}

void Field::compact(int y1, int y2)
{
	//check for border validity
	if(y1 < 0 || y2 < 0 || y1 >= height || y2 >= height)
		return;

	bool y1empty=true;
	bool y2empty=true;

	//Figure out which row needs to be deleted
	for(int x=0;x<width;++x)
	{
		if(numbers[y1][x] != 0)
			y1empty=false;
		if(numbers[y2][x] != 0)
			y2empty=false;
	}

	//there is nothing to do
	if(!y1empty && !y2empty)
		return;

	//deletes row y and shifts all rows above it down one position
	auto shiftrows = [&](int y){
		delete[] numbers[y];
		for(int i = y+1; i < height;++i)
			numbers[i-1] = numbers[i];
		--height;
	};

	//If both rows are empty, the higher one needs to be removed first
	if(y1empty && y2empty)
	{
		if(y1 == y2)
			shiftrows(y1);
		else if (y1 > y2)
		{
			shiftrows(y1);
			shiftrows(y2);
		} else {
			shiftrows(y2);
			shiftrows(y1);
		}
	}//At this point ONE of them needs to be removed
	else if(y1empty)
		shiftrows(y1);
	else
		shiftrows(y2);
}

bool Field::remove(int x1, int y1, int x2, int y2)
{
	//Check for general permission (borders and Field Values)
	if (x1<0 || x2<0 || y1<0 || y2<0 || x1>= width || y1>=height || x2>=width || y2>=height ||
	   (x1==x2 && y1==y2) || numbers[y1][x1]==0 || numbers[y2][x2]==0 ||
	   !(numbers[y1][x1]==numbers[y2][x2] || numbers[y1][x1]+numbers[y2][x2]==10) )
		return false;

	//Check wether the Fields border each other
	// i iterates through the directions
	int x,y;
	for(char i=0;i<4;++i)
	{
		next(i,x1,y1,x,y);

		if(x==x2 && y==y2)
		{
			numbers[y1][x1] = 0;
			numbers[y2][x2] = 0;
			count -= 2;
			compact(y1,y2);
			return true;
		}
	}

	return false;
}

bool Field::checkformove(){
	int u,i,a,e;
	generatemove(u,i,a,e);
	return u != -1;
}

void Field::generatemove(int& mx1,int & my1, int& mx2, int& my2)
{
	char s;
	for(int tx,ty,x,y=0;y<height;++y)
	{
		for (x=0;x<width;++x)
		{
			//left and up dont need to be done because moves are symmetric
			for(s=2;s<4;++s){
				next(s,x,y,tx,ty);
					//Exclude the same field and values of 0
				if( (!(x==tx && y==ty)) && numbers[y][x] != 0 &&
					//Equal or ten together
					(numbers[y][x]==numbers[ty][tx] || ((numbers[y][x]+numbers[ty][tx])==10) ) )
				{
					mx1 = x;
					my1 = y;
					mx2 = tx;
					my2 = ty;
					return;
				}
			}
		}
	}
	mx1 = -1;
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

	std::cout<<"Saving to: "<<filename<<std::endl;
	std::ofstream outfile(filename);

	if(!outfile.is_open())
		throw -5;

	//Write Metadata
	outfile<<width<<" "<<height<<std::endl;

	//Write the numbers to the file
	for(int x,y=0;y<height;++y)
		for(x=0;x<width;++x)
			outfile.put(numbers[y][x]+'0');


	outfile.flush();
	outfile.close();
}

void Field::next(char dir, int x, int y, int& newx, int& newy)
{
	switch(dir)
	{
		case 0:
			nextleft(x,y,newx,newy);
			break;
		case 1:
			nextup(x,y,newx,newy);
			break;
		case 2:
			nextright(x,y,newx,newy);
			break;
		case 3:
			nextdown(x,y,newx,newy);
	}
}

void Field::nextleft(int x,int y, int& newx, int& newy)
{
	do{
		if(--x < 0)
		{
			x= width-1;
			--y;
		}
	} while (y >= 0 && numbers[y][x]==0);
	if (y < 0)
		++y;
	newx = x;
	newy = y;
}

void Field::nextright(int x, int y, int& newx, int& newy)
{
	do{
		if(++x == width)
		{
			x = 0;
			++y;
		}
	} while (y < height && numbers[y][x]==0);
	if (y == height)
		--y;
	newx = x;
	newy = y;
}

void Field::nextup(int x, int y, int& newx, int& newy)
{
	do
		--y;
	while(y>0 && numbers[y][x]==0);
	if (y < 0)
		++y;
	newx = x;
	newy = y;
}

void Field::nextdown(int x, int y, int& newx, int& newy)
{
	do
		++y;
	while (y < height && numbers[y][x]==0);
	if (y == height)
		--y;
	newx = x;
	newy = y;
}