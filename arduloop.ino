#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include "Arduboy2.h"

#include "global.h"
#include "song.h"
#include "player.h"

#include "home.h"
#include "trackeditor.h"
#include "instrumenteditor.h"
#include "songeditor.h"

int screen = SCREEN_HOME;
int screendepth = 0;

void switchScreen(int newscreen, int forward) {
  if (screen == SCREEN_HOME) home_leave(forward);
  else if (screen == SCREEN_INSTRUMENT) instrumenteditor_leave(forward);
  else if (screen == SCREEN_TRACK) trackeditor_leave(forward);
  else if (screen == SCREEN_SONG) songeditor_leave(forward);

  if (newscreen != screen) {
    screen = newscreen;
    if (forward) {
      screendepth ++;
    }
  }

  if (screen == SCREEN_HOME) home_enter(forward);
  else if (screen == SCREEN_INSTRUMENT) instrumenteditor_enter(forward);
  else if (screen == SCREEN_TRACK) trackeditor_enter(forward);
  else if (screen == SCREEN_SONG) songeditor_enter(forward);
}

int slidePosition(int step) {
  int x = 0;
  if (step < MIN_SLIDE) { step = MIN_SLIDE; }
  if (step > MAX_SLIDE) { step = MAX_SLIDE; }
  // return step * 8;
  if (step < 0) {
    int p = -step * 8; // 0 - 127
    x = sine_wave[p]; // 128 - 255
    x = (128 - x) / 4; // 0 - -32
  }
  if (step > 0) {
    int p = step * 8; // 0 - 127
    x = sine_wave[p] - 128; // 128 - 255
  }
  return x;
}

Arduboy2 arduboy;
Arduboy2 *_arduboy = &arduboy;

#define REPEAT_INTERVAL 100

long last_down_rep = 0;
int last_down_repeat_counter = 0;

// #define SHIFT 3

// struct BALL {
//   int x, y, dx, dy;
//   int s;
// };

// #define NUMBALLS 8

// struct BALL balls[NUMBALLS];

void waitStart() {
  arduboy.clear();

  arduboy.drawBitmap(0, 0, __logo, 128, 64, WHITE);

  // arduboy.setCursor(8, 24);
  // arduboy.print("DRUMS");
  arduboy.setCursor(8, 32);
  arduboy.print("PRESS ANY KEY");

  arduboy.display();

  int button_pressed = 0;
  while(!button_pressed) {
    if (arduboy.pressed(LEFT_BUTTON) ||
        arduboy.pressed(RIGHT_BUTTON) ||
        arduboy.pressed(UP_BUTTON) ||
        arduboy.pressed(DOWN_BUTTON) ||
        arduboy.pressed(A_BUTTON) ||
        arduboy.pressed(B_BUTTON)) {
      button_pressed = 1;
    }
  }

  arduboy.clear();
}


void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.display();




  // for (int i = 0; i < NUMBALLS; i++) {
  //   BALL *b = (BALL *)&balls[i];
  //   b->x = rand() % 127;
  //   b->y = rand() % 60;
  //   int s = 1 + (rand() % 6);
  //   b->s = i % 2;
  //   b->x <<= SHIFT;
  //   b->y <<= SHIFT;
  //   b->dx = s * (-1 + ((rand() % 2) * 3));
  //   b->dy = s * (-1 + ((rand() % 2) * 3));
  // }

  clearSong();
  loadSong();

  home_init();
  trackeditor_init();
  instrumenteditor_init();
  songeditor_init();

  // waitStart();

  switchScreen(SCREEN_HOME, true);
  startTimers();
}

void drawbar(int x, char *label, int value, int y, int selected) {
  arduboy.setCursor(x + 16, y);
  arduboy.print(label);

  if (selected) {
    arduboy.fillRect(x + 40, y, 64 + 4, 8, 1);
    arduboy.fillRect(x + 41, y+1, 64 + 2, 6, 0);
  }

  if (value < 1) value = 1;
  if (value > 31) value = 31;
  arduboy.fillRect(x + 42, y+2, value * 2, 4, 1);
}

void checkButton(int button) {
  long T = millis();
  long d = T - last_down_rep;
  if (arduboy.pressed(button)) {
    volume = 1;
    if (d > REPEAT_INTERVAL * 2) {
      last_down_repeat_counter = 0;
    }
    if (d > REPEAT_INTERVAL) {
      if (screen == SCREEN_HOME) home_input(button, last_down_repeat_counter);
      else if (screen == SCREEN_INSTRUMENT) instrumenteditor_input(button, last_down_repeat_counter);
      else if (screen == SCREEN_TRACK) trackeditor_input(button, last_down_repeat_counter);
      else if (screen == SCREEN_SONG) songeditor_input(button, last_down_repeat_counter);
      last_down_rep = T;
      last_down_repeat_counter ++;
    }
  }
}

void loop()
{
  home_update();
  instrumenteditor_update();
  trackeditor_update();
  songeditor_update();

  if (arduboy.nextFrame()) {
    arduboy.clear();

    home_render();
    instrumenteditor_render();
    trackeditor_render();
    songeditor_render();

    // arduboy.setCursor(0, rand() % 100);
    // arduboy.print("Hello World!");

    // int maxwidth = 128 << SHIFT;
    // int maxheight = 64 << SHIFT;
    // for (int i = 0; i < NUMBALLS; i++) {
    //   BALL *b = (BALL *)&balls[i];
    //   int x = b->x >> SHIFT;
    //   int y = b->y >> SHIFT;
    //   arduboy.fillRect(x - 1, y - 1, 3, 3, 0);
    //   arduboy.drawPixel(x, y, 1);
    //   if (b->s == 1) {
    //     arduboy.drawPixel(x - 1, y, 1);
    //     arduboy.drawPixel(x, y - 1, 1);
    //     arduboy.drawPixel(x + 1, y, 1);
    //     arduboy.drawPixel(x, y + 1, 1);
    //   }

    //   b->x += b->dx;
    //   b->y += b->dy;
    //   if (b->x > maxwidth) {
    //     b->dx = -abs(b->dx);
    //   }
    //   if (b->y > maxheight) {
    //     b->dy = -abs(b->dy);
    //   }
    //   if (b->x < 0) {
    //     b->dx = abs(b->dx);
    //   }
    //   if (b->y < 0) {
    //     b->dy = abs(b->dy);
    //   }
    // }

    arduboy.display();
  }

  checkButton(LEFT_BUTTON);
  checkButton(RIGHT_BUTTON);
  checkButton(UP_BUTTON);
  checkButton(DOWN_BUTTON);
  checkButton(A_BUTTON);
  checkButton(B_BUTTON);
}


