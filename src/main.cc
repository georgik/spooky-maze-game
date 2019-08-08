#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <cmath>
#include <iostream>

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

string	scroll;
FONT	*myfont;

BITMAP	*scr;
BITMAP	*mixer;
BITMAP	*submixer;
BITMAP  *maze;
BITMAP	*stena;
BITMAP	*podlaha;
BITMAP	*ghost[2];
BITMAP	*darceky[10];
BITMAP	*empty;
BITMAP	*smiley;
BITMAP	*teleport;
BITMAP	*sranda;
BITMAP	*walker;


int midivolume, wavvolume;
int boomx, boomy;

DATAFILE *data;
DATAFILE *data2;

PALETTE pal;

BITMAP *background;

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


int PlayMuzak()
{
	int i;

	if((midi_pos>0) || (!midigo)) { return 1; }
	
	switch(random()%5) {
		case 0 : { i = music01; break; }
		case 1 : { i = music02; break; }
		case 2 : { i = music03; break; }
		case 3 : { i = music04; break; }
		case 4 : { i = music05; break; }
	}
	play_midi((MIDI *)data[i].dat, 0);
	return 0;
}

class MAZER
{
	public:
	int x;
	int y;
	int tex;
	int dir;
	int visible;

	void Place();
	int Move(int direct);
	void MoveWalls(int direct);
	void MoveWhereYouWant(MAZER& player);
	int Collision(MAZER& m);
};

void MAZER::Place()
{
	do {
		x = random() % w;
		y = random() % h;
	} while(!EmptyMaze(x,y));
	dir = 0;
	visible = 1;
	x = x * SPRITEX;
	y = y * SPRITEY;
}

void MAZER::MoveWalls(int direct)
{
	int mx = x / SPRITEX;
	int my = y / SPRITEY;
	
	
	if((x % SPRITEX == 0) && (y % SPRITEY == 0)) {	
		dir = NONE;
		switch (direct) {
			case LEFT: {
				if(EmptyMazeNoWalls(mx-1, my)) { 
					x -= MOTIONX;
					dir = LEFT; 
				}
				break;
			}
			case RIGHT: {
				if(EmptyMazeNoWalls(mx+1, my)) { 
					x += MOTIONX;
					dir = RIGHT; 
				}
				break;
			}
			case UP: {
				if(EmptyMazeNoWalls(mx, my-1)) { 
					y -= MOTIONY;
					dir = UP; 
				}
				break;
			}
			case DOWN: {
				if(EmptyMazeNoWalls(mx, my+1)) { 
					y += MOTIONY;
					dir = DOWN; 
				}
				break;
			}
		}
	} else {
		switch(dir) {
			case UP : {
				y -= MOTIONY;
				break;
			}
			case DOWN : {
				y += MOTIONY;
				break;
			}
			case LEFT : {
				x -= MOTIONX;
				break;
			}
			case RIGHT : {
				x += MOTIONX;
				break;
			}
		}
	}

}

int MAZER::Move(int direct)
{
	int mx = x / SPRITEX;
	int my = y / SPRITEY;
	
	
	if((x % SPRITEX == 0) && (y % SPRITEY == 0)) {	
		dir = NONE;
		switch (direct) {
			case LEFT: {
				if(EmptyMaze(mx-1, my)) { 
					x -= MOTIONX;
					dir = LEFT;
					return 1;
				}
				break;
			}
			case RIGHT: {
				if(EmptyMaze(mx+1, my)) { 
					x += MOTIONX;
					dir = RIGHT; 
					return 1;
				}
				break;
			}
			case UP: {
				if(EmptyMaze(mx, my-1)) { 
					y -= MOTIONY;
					dir = UP; 
					return 1;
				}
				break;
			}
			case DOWN: {
				if(EmptyMaze(mx, my+1)) { 
					y += MOTIONY;
					dir = DOWN; 
					return 1;
				}
				break;
			}
		}
	} else {
		switch(dir) {
			case UP : {
				y -= MOTIONY;
				break;
			}
			case DOWN : {
				y += MOTIONY;
				break;
			}
			case LEFT : {
				x -= MOTIONX;
				break;
			}
			case RIGHT : {
				x += MOTIONX;
				break;
			}
		}
		return 1;
	}
	return 0;
}

int MAZER::Collision(MAZER& m)
{
	if((x / SPRITEX == m.x / SPRITEX) &&
		(y / SPRITEY == m.y / SPRITEY)) {
		return 1;
	} else {
		return 0;
	}
}

void MAZER::MoveWhereYouWant(MAZER& player)
{
	int dx,dy;
	int retval;
	
	dx = player.x-x;
	dy = player.y-y;
	if (abs(dx) > abs(dy)) {
		if (dx > 0) retval = Move(RIGHT); else retval = Move(LEFT);
	}
	else {
		if (dy > 0) retval = Move(DOWN); else retval = Move(UP);
	} 
	if (!retval) Move(random()%4+1);
}

MAZER player;
MAZER obj[MaxObj];
MAZER srandaci[MaxSrandaci];
MAZER walkeri[MaxWalker];
MAZER dynamit;

void PlaceObj()
{
	int i;
	player.Place();
	for(i=0; i!=MaxObj; i++) {
		obj[i].Place();
	}
	for(i=0; i!=MaxSrandaci; i++) {
		srandaci[i].Place();
	}
	for(i=0; i!=MaxWalker; i++) {
		walkeri[i].Place();
	}
	dynamit.Place();

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


void PrintMap(BITMAP *bmp)
{
	int i;
	for(i=0; i!=MaxMapX; i++) {
		if(Map[i]) { 
			putpixel(bmp, i % 35, i / 35, WHITE); 
		} else {
			putpixel(bmp, i % 35, i / 35, BLACK); 
		}
	}
}

void PrintOne(BITMAP *bmp, int x, int y, int i)
{
	rectfill(bmp, x * SPRITEX, y* SPRITEY, 
		(x + 1) * SPRITEX, (y + 1) * SPRITEY,
		i);

}

void PrintOne(BITMAP *bmp, int x, int y, BITMAP *img)
{
	draw_sprite(bmp, img, x * SPRITEX, y* SPRITEY);

}

void PrintBrick(int type, BITMAP *bmp, int x, int y)
{
	switch(type) {
		case FLOOR: {
			PrintOne(bmp, x, y, podlaha);
			break;
		}
		case WALL : {
			PrintOne(bmp, x, y, stena);
			break;
		}
		case EXPLODED_FLOOR : {
			PrintOne(bmp, x, y, (BITMAP *)data[ipodlaha2].dat);
			break;
		 }
		case EXPLODING_WALL : {
			PrintOne(bmp, x, y, (BITMAP *)data[iexplozia1].dat);
			break;
		 }
		case EXPLODING_FLOOR : {
			PrintOne(bmp, x, y, (BITMAP *)data[iexplozia2].dat);
			break;
		 }

	}
}

void PrintMap(BITMAP *bmp, int x1, int y1, int x2, int y2)
{
	int a,b;
	for(a=y1; a!=y2; a++) {
		for(b=x1; b!=x2; b++) {
			if((a < 0) || (b < 1) ||
				(a > w - 1) || (b > h - 2)) {
				PrintOne(bmp, b - x1, a - y1, empty);
			} else {
				PrintBrick(Map[a * h + b], bmp, b - x1, a - y1);
			}
		}
	}
				
}

void NewGame()
{
	int i;
	dynacount = 0;
	done = 0;
	walktime = 0;
	bzero(Map, MaxMapX);
	MakeMap(1);

	for(i = 0; i!=MaxMapX; i++) {
		Map[i]=!Map[i];
	} //Inversion of maze

	PlaceObj();
	teleport_energy = MAXENERGY;
}

int Try(int d, int x, int y)
{
	set_color_depth(d);
	return !set_gfx_mode(GFX_AUTODETECT, x, y, 0, 0);
}

int Init()
{
	char *buf;

	set_uformat(U_ASCII); 

	srand(time(0));
	
	allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();
	if(Try(16, 320, 200)) {
		screen_start_x = 0;
		screen_start_y = 0;
	} else
	if(Try(16, 640, 480)) {
		screen_start_x = 160;
		screen_start_y = 140;
	} else	
	if(Try(8, 320, 200)) {
		screen_start_x = 0;
		screen_start_y = 0;
	} else {	
		cerr<<"GFX error: "<<allegro_error<<endl;
		allegro_exit();
		exit(1);
	}
	
	if(install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0)) {
		cerr<<"No SFX :("<<endl;
	}

	wavvolume = 255;
	midivolume = 150;

	set_volume(255, 150);
	midigo = 1;
	
	b[0]=-w;      //Directions to generate maza
	b[1]=w;
	b[2]=1;
	b[3]=-1;
 
	//text_mode(-1);

	
	scr = create_bitmap(scrx, scry);
	clear(scr);

	if(!(data = load_datafile("spooky.dat"))) {
		cerr<<"Unable to open datafile!"<<endl;
		allegro_exit();
		exit(1);
	}
	
	PlayMuzak();
	mixer = (BITMAP *)data[iline].dat;
	submixer = create_bitmap(370, 30);
	blit(mixer, submixer, 0, 0, 0, 0, 370, 30);

	maze = create_bitmap(scrx, scry);


	empty =		(BITMAP *)data[iempty].dat;
	teleport =	(BITMAP *)data[iteleport].dat;
	ghost[0] =	(BITMAP *)data[ighost1].dat;
	ghost[1] =	(BITMAP *)data[ighost2].dat;
	darceky[0] =	(BITMAP *)data[idarcek0].dat;
	darceky[1] =	(BITMAP *)data[idarcek0].dat;
	stena =		(BITMAP *)data[istena].dat;
	smiley =	(BITMAP *)data[ismiley].dat;
	podlaha =	(BITMAP *)data[ipodlaha].dat;
	background = 	(BITMAP *)data[iback].dat;
	sranda =	(BITMAP *)data[isranda].dat;
	walker =	(BITMAP *)data[iwalker].dat;
	
	myfont = (FONT *)data[times24].dat;

	buf = (char *)data[tscroll].dat;
	
	scroll = "       ";
	scroll = buf;
	scroll += "\\";

	if(!myfont) { myfont = font; }
	
	bzero(xbuf, 3);
	
	char_counter = 8;
	char_pointer = 0;


	
	NewGame();
	
	
	return 0;
}

int Done()
{
	int i;

	destroy_bitmap(scr);
	destroy_bitmap(maze);
	destroy_bitmap(submixer);
	
	unload_datafile(data);

	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	
	stop_midi();	
	remove_sound();
	remove_keyboard();
	remove_timer();
	remove_mouse();

	allegro_exit();
	return 0;
}



void DrawScroll(BITMAP *bmp)
{
	char_counter--;

	if(!char_counter) {
		char_pointer++;
		if(scroll[char_pointer] == '\\') { char_pointer = 0; }
		xbuf[0] = scroll[char_pointer];
		char_counter = text_length(myfont, xbuf) / 2 + 1; 
		textprintf(mixer, myfont, scrx + 1, 0, WHITE, "%c", scroll[char_pointer]);
	}
	
	blit(mixer, submixer, 2, 0, 0, 0, scrx - 2 + 50, 30);
	blit(submixer, mixer, 0, 0, 0, 0, scrx + 50, 30);
	masked_blit(mixer, bmp, 0, 0, 0, 170, scrx, 30);
}

int counter = 0;
int i, sx, sy;

void AddSomeObjects()
{
	int i,c = 0;
	for(i=0; i!=MaxObj; i++) {
		if(!obj[i].visible) {
			obj[i].Place();
			c++;
		}
		if(c==5) { break; }
	}
}

void Boom()
{
	int x = player.x / SPRITEX;
	int y = player.y / SPRITEY;

	boomtime = 10;
	boomx = x;
	boomy = y;
	
	SetMazeExploding(x-1, y-1);
	SetMazeExploding(x, y-1);
	SetMazeExploding(x+1, y-1);
	SetMazeExploding(x-1, y);
	SetMazeExploding(x+1, y);
	SetMazeExploding(x-1, y+1);
	SetMazeExploding(x, y+1);
	SetMazeExploding(x+1, y+1);
}

void BoomStop()
{
	int x = boomx;
	int y = boomy;

	SetMaze(x-1, y-1);
	SetMaze(x, y-1);
	SetMaze(x+1, y-1);
	SetMaze(x-1, y);
	SetMaze(x+1, y);
	SetMaze(x-1, y+1);
	SetMaze(x, y+1);
	SetMaze(x+1, y+1);
}


void Game()
{
	int visiblecount;

	if(teleport_energy < MAXENERGY) { 
		teleport_energy++; 
		if (teleport_energy == MAXENERGY) {
			play_sample((SAMPLE *)data[steleready].dat, 255, 128, 1000, 0);
		}
	}
	
	clear(scr);	

	clear(maze);

	PrintMap(maze, player.x / SPRITEX - 9, player.y / SPRITEY - 5, 
		player.x / SPRITEX + 12, player.y / SPRITEY + 7);

	sx = player.x % SPRITEX;
	sy = player.y % SPRITEY;
	
	blit(maze, scr, sx, sy, 0, 0,
		scrx , scry );

	counter++;
	if(counter == 20) { counter = 0; }

	draw_sprite(scr, ghost[counter / 10], SPRITEX * 9, SPRITEY *5);
	
	sx = player.x;
	sy = player.y;
	
	visiblecount = 0;
	
	for(i=0; i!=MaxObj; i++) {

		if(obj[i].visible) { visiblecount++; }
		
		if((obj[i].visible) && 
			(obj[i].x > player.x - 10 * SPRITEX) &&
			(obj[i].y > player.y - 6 * SPRITEY) &&
			(obj[i].x < player.x + 10 * SPRITEX) &&
			(obj[i].y < player.y + 5 * SPRITEY)) {

			draw_sprite(scr, darceky[obj[i].tex],
				obj[i].x + 9 * SPRITEX - sx, 
				obj[i].y + 5 * SPRITEY- sy);
		}
		if((obj[i].visible) && (player.Collision(obj[i]))) {
			play_sample((SAMPLE *)data[spick].dat, SNDVOL, 128, 1000, 0);
			obj[i].visible = 0;
		}
	}

	for(i=0; i!=MaxSrandaci; i++) {

		srandaci[i].MoveWhereYouWant(player);
		
		if((srandaci[i].visible) && 
			(srandaci[i].x > player.x - 10 * SPRITEX) &&
			(srandaci[i].y > player.y - 6 * SPRITEY) &&
			(srandaci[i].x < player.x + 10 * SPRITEX) &&
			(srandaci[i].y < player.y + 5 * SPRITEY)) {

			draw_sprite(scr, sranda,
				srandaci[i].x + 9 * SPRITEX - sx, 
				srandaci[i].y + 5 * SPRITEY- sy);
		}
		if(player.Collision(srandaci[i])) {
			play_sample((SAMPLE *)data[ssrandak].dat, SNDVOL, 128, 1000, 0);
			AddSomeObjects();
			player.Place();
			srandaci[i].Place();
		}
	}

	for(i=0; i!=MaxWalker; i++) {

		if((walkeri[i].visible) && 
			(walkeri[i].x > player.x - 10 * SPRITEX) &&
			(walkeri[i].y > player.y - 6 * SPRITEY) &&
			(walkeri[i].x < player.x + 10 * SPRITEX) &&
			(walkeri[i].y < player.y + 5 * SPRITEY)) {

			draw_sprite(scr, walker,
				walkeri[i].x + 9 * SPRITEX - sx, 
				walkeri[i].y + 5 * SPRITEY- sy);
		}
		if(player.Collision(walkeri[i])) {
			play_sample((SAMPLE *)data[swalker].dat, SNDVOL, 128, 1000, 0);
			walkeri[i].Place();
			walktime += JohnyWalker;
		}
	}

	if((dynamit.visible) && 
		(dynamit.x > player.x - 10 * SPRITEX) &&
		(dynamit.y > player.y - 6 * SPRITEY) &&
		(dynamit.x < player.x + 10 * SPRITEX) &&
		(dynamit.y < player.y + 5 * SPRITEY)) {

		draw_sprite(scr, (BITMAP *)data[idynamit].dat,
			dynamit.x + 9 * SPRITEX - sx, 
			dynamit.y + 5 * SPRITEY- sy);
	}
	if(player.Collision(dynamit)) {
			dynacount++;
			play_sample((SAMPLE *)data[spick].dat, SNDVOL, 128, 1000, 0);
			dynamit.Place();
	}

	if(boomtime) { 
		if (boomtime == 1) { BoomStop(); }
		boomtime--; 
	}

	if(walktime) { walktime--; }
	if(walktime == 1) { 
		play_sample((SAMPLE *)data[snowalker].dat, SNDVOL, 128, 1000, 0);
	}
	if (!visiblecount) { 
		done = 1; 
		counter = 200; 
		clear_keybuf();
		visiblecount = -1;
	}

	draw_sprite(scr, background, 0, 0);
	draw_sprite(scr, darceky[0], 0, 0);
	draw_sprite(scr, teleport, 0, 30);
	draw_sprite(scr, walker, 0, 50);
	draw_sprite(scr, (BITMAP *)data[idynamit].dat, 0, 70);

	if(midigo) { 
		draw_sprite(scr, (BITMAP *)data[isound].dat, 300, 0);
	} else {
		draw_sprite(scr, (BITMAP *)data[inosound].dat, 300, 0);
	}
	
	masked_blit((BITMAP *)data[ivolume].dat, scr, 0, 0, 300 , 20, wavvolume / 15, 16);
	
	textprintf(scr, myfont, 20, 0, WHITE, "%d", visiblecount);
	textprintf(scr, myfont, 20, 30, WHITE, "%d", teleport_energy / 10);
	textprintf(scr, myfont, 20, 50, WHITE, "%d", walktime / 10);
	textprintf(scr, myfont, 20, 70, WHITE, "%d", dynacount);
	
}

void Draw()
{
	if(done) {
		if(!counter) {
			if(keypressed()) {
				done = 0;
				NewGame();
			}
		} else {
			counter--;
			clear_keybuf();
		}

		draw_sprite(maze, smiley, random()%scrx, random()%scry);
		blit(maze, scr, 0, 0, 0, 0,
			scrx , scry );
		draw_sprite(scr, background, 0, 0);
	} else {
		Game();
	}
	DrawScroll(scr);
	blit(scr,screen,0,0,screen_start_x,screen_start_y,scrx,scry);		
}


int Run()
{
	int Stop = 0;

	do {
		PlayMuzak();
		Draw();

		if((key[KEY_ENTER]) && (teleport_energy == MAXENERGY)){ 
			play_sample((SAMPLE *)data[steleport].dat, SNDVOL, 128, 1000, 0);
			player.Place(); 
			usleep(SLEEP_SWEAT_DREAMS);
			teleport_energy = 0;
			clear_keybuf();
		}
		
		if(key[KEY_UP]) { 
			if(walktime) { 
				player.MoveWalls(UP); 
			} else { player.Move(UP);} 
		} else
		if(key[KEY_DOWN]) { 
			if(walktime) {
				player.MoveWalls(DOWN);
			} else {
				player.Move(DOWN); 
			}
		} else
		if(key[KEY_LEFT]) { 
			if(walktime) {
				player.MoveWalls(LEFT);
			} else {
				player.Move(LEFT); 
			}
		} else
		if(key[KEY_RIGHT]) { 
			if(walktime) {
				player.MoveWalls(RIGHT); 
			} else {
				player.Move(RIGHT); 
			}
		} else { 
			if(walktime) {
				player.MoveWalls(NONE); 
			} else {
				player.Move(NONE);
			}
		}


		if(key[KEY_PLUS_PAD]) { 
			if(wavvolume<250) { 
				wavvolume += 5; 
				set_volume(wavvolume, midivolume);
			}
		}
		if(key[KEY_MINUS_PAD]) {
			if(wavvolume>5) { 
				wavvolume -= 5; 
				set_volume(wavvolume, midivolume);
			}
		}
		if(key[KEY_ASTERISK]) {
			usleep(SLEEP_SWEAT_DREAMS);
			clear_keybuf();
			midigo = !midigo;
			if(!midigo) { stop_midi(); }
		}
		
		if((key[KEY_SPACE]) && (dynacount) && (!boomtime)) {
			dynacount--;
			usleep(SLEEP_SWEAT_DREAMS);
			clear_keybuf();
			Boom();
			play_sample((SAMPLE *)data[sexplode].dat, SNDVOL, 128, 1000, 0);
		}
		

			
		if(key[KEY_ESC]) { Stop = 1; }
		usleep(SLEEP_SWEAT_DREAMS);
		
	} while(!Stop);
	
	return 0;
}

int main(int argc, char **argv)
{
	Init();
	Run();
	Done();
	return 0;
}

#ifdef LINUX
END_OF_MAIN();
#endif

