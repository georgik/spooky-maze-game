#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
    using std::ofstream;

using std::string;
using std::cerr;
using std::endl;

#include "data.h"

#define	scrx	320
#define scry	200

#define SLEEP_SWEAT_DREAMS	10000

#define RGB(a, b, c) (a)<<11 | (b)<<5 | (c)

#define WHITE	RGB(255, 255, 255)
#define BLACK	RGB(0, 0, 0)
#define GRAY	RGB(25, 25, 25)
#define BLUE	RGB(0, 0, 255)

#define	MaxMapX	63*63
#define MaxMapY	63

#define NONE	0
#define UP	1
#define DOWN	2
#define LEFT	3
#define RIGHT	4

#define SNDVOL	200

#define MaxObj		99
#define MaxSrandaci	5
#define MaxWalker	5
#define JohnyWalker	500

#define time_delay 2

#define SPRITEX	16
#define SPRITEY 16

#define MOTIONX	2
#define MOTIONY	2

#define MAXENERGY 1000

#define FLOOR		0
#define	WALL		1
#define EXPLODING_FLOOR	2
#define EXPLODING_WALL	3
#define EXPLODED_FLOOR	-1


int midivolume, wavvolume;
int boomx, boomy;


int char_pointer;
int char_counter;

int teleport_energy;
int walktime;

char xbuf[3];

int b[4];
int dynacount;
int Map[MaxMapX];

int h = 63;
int w = 63;

int midigo;

int boomtime = 0;
int done;

int screen_start_x;
int screen_start_y;


int EmptyMaze(int x, int y)
{
	if((x<1) || (y<0) || (x>w-2) || (y>h-1)) { 
		return 0;
	}
	if(Map[x + y * w] < 1) {
		return 1;
	} else {
		return 0;
	}
}

void SetMaze(int x, int y)
{
	if(!((x<1) || (y<0) || (x>w-2) || (y>h-1))) { 
		Map[x + y * w] = EXPLODED_FLOOR;
	}
}

void SetMazeExploding(int x, int y)
{
	if(!((x<1) || (y<0) || (x>w-2) || (y>h-1))) { 
		if(Map[x + y * w] == FLOOR) {
			Map[x + y * w] = EXPLODING_FLOOR;
		} else {
			Map[x + y * w] = EXPLODING_WALL;
		}
	}
}



int EmptyMazeNoWalls(int x, int y)
{
	if((x<1) || (y<0) || (x>w-2) || (y>h-1)) { 
		return 0;
	}
	return 1;
}



void MakeMap(int p)
{
	unsigned char i, d;	
	Map[p] = 1;
	
	do {
		d = 0;
		if ((p > 2 * w) && (!(Map[p - 2 * w ])))       d++;
		if ((p < w * (h - 2)) && (!(Map[p + 2 * w]))) d+=2;
		if (((p % w) != (w - 2)) && (!(Map[p + 2])))  d+=4;
		if (((p % w) != 1) && (!(Map[p - 2])))        d+=8;
		if (d != 0) {
			do {
				i = random()%4;
			} while (!(d & (1 << i)));
			Map[p + b[i]] = 1;
			MakeMap(p + 2 * b[i]);
		}
	} while (d != 0);


}


// void PrintMap(BITMAP *bmp)
// {
// 	int i;
// 	for(i=0; i!=MaxMapX; i++) {
// 		if(Map[i]) { 
// 			putpixel(bmp, i % 35, i / 35, WHITE); 
// 		} else {
// 			putpixel(bmp, i % 35, i / 35, BLACK); 
// 		}
// 	}
// }



void PrintMap(int x1, int y1, int x2, int y2)
{
	ofstream stream;
	stream.open("maze.txt");
	int a,b;
	for(a=y1; a!=y2; a++) {
		stream << "[";
		for(b=x1; b!=x2; b++) {
			// if((a < 0) || (b < 1) ||
			// 	(a > w - 1) || (b > h - 2)) {
			// 	stream << "0,";
			// } else {
				if (Map[a*63+b]) {
				stream << "0,";
				} else {
				stream << "1,";
				}
			// }
		}
		stream << "]," << endl;
	}
	stream << endl;
				
}

void NewGame()
{
	int i;
	dynacount = 0;
	done = 0;
	walktime = 0;
	for(i = 0; i!=MaxMapX; i++) {
		Map[i] = 0;
	}
	MakeMap(1);

	// for(i = 0; i!=MaxMapX; i++) {
	// 	Map[i]=!Map[i];
	// } //Inversion of maze


	teleport_energy = MAXENERGY;
}


int Init()
{
	char *buf;

	srand(time(0));
	

	
	b[0]=-w;      //Directions to generate maza
	b[1]=w;
	b[2]=1;
	b[3]=-1;
 
	//text_mode(-1);






	
	NewGame();
	PrintMap(0,0,63,63);
	
	
	return 0;
}



int counter = 0;
int i, sx, sy;



int main(int argc, char **argv)
{
	Init();
	
	return 0;
}
