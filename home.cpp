#include "global.h"
#include "home.h"
#include "song.h"
#include "player.h"

int home_s;
int home_ts;
int home_intro;
int home_selection;

void home_init() {
  home_s = 0;
  home_ts = 0;
  home_selection = 0;
  home_intro = 40;
}

void home_enter(int forward) {
  home_ts = 0;
}

void home_leave(int forward) {
  home_ts = forward ? MIN_SLIDE : MAX_SLIDE;
}

void home_input(int button, int repeat) {
  if (button == A_BUTTON && repeat == 0) {
    switchScreen(SCREEN_HOME, true);
  }

  if (button == B_BUTTON && repeat == 0) {
    switchScreen(SCREEN_HOME, false);
  }

  if (button == A_BUTTON && repeat == 0) {
    if (home_selection == 0) switchScreen(SCREEN_TRACK, true);
    if (home_selection == 1) switchScreen(SCREEN_INSTRUMENT, true);
    if (home_selection == 2) switchScreen(SCREEN_SONG, true);
  }

  if (button == UP_BUTTON && repeat == 0) {
    home_selection --;
    if (home_selection < 0) {
      home_selection = 2;
    }
  }

  if (button == DOWN_BUTTON && repeat == 0) {
    home_selection ++;
    if (home_selection > 2) {
      home_selection = 0;
    }
  }
}

void home_render() {
  if (home_s < home_ts) home_s ++;
  if (home_s > home_ts) home_s --;

  if (home_s > MIN_SLIDE && home_s < MAX_SLIDE) {
    int bx = slidePosition(home_s);

    _arduboy->drawBitmap(bx, 0, __logo, 128, 64, WHITE);

    _arduboy->fillRect(bx + 98 + home_intro, 13, 90, 44, 0);
    _arduboy->drawRect(bx + 100 + home_intro, 15, 88, 40, 1);

    _arduboy->setCursor(bx + 105 + home_intro, 20);
    if (home_selection == 0 && (player_step & 3) == 0) {
      _arduboy->print("TRK<");
    } else {
      _arduboy->print("TRK");
    }

    _arduboy->setCursor(bx + 105 + home_intro, 30);
    if (home_selection == 1 && (player_step & 3) == 0) {
      _arduboy->print("INS<");
    } else {
      _arduboy->print("INS");
    }

    _arduboy->setCursor(bx + 105 + home_intro, 40);
    if (home_selection == 2 && (player_step & 3) == 0) {
      _arduboy->print("SNG<");
    } else {
      _arduboy->print("SNG");
    }

    if (home_intro > 0) {
      home_intro -= 2;
    }
  }
}

void home_update() {
  if (home_s == 0) {
    if (_song->dirty) {
      saveSong();
    }
  }
}
