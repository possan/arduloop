#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Arduboy2.h"
#include "images.h"

#define SCREEN_HOME 20
#define SCREEN_INSTRUMENT 40
#define SCREEN_TRACK 50
#define SCREEN_SONG 60

extern Arduboy2 *_arduboy;

extern int screen;
extern int screendepth;

#define MIN_SLIDE -8
#define MAX_SLIDE 8
extern signed int slidePosition(signed int step);

extern void switchScreen(int newscreen, int forward);

extern void drawbar(int x, char *label, int value, int y, int selected);

#endif
