#include "global.h"
#include "songeditor.h"
#include "player.h"
#include "song.h"

int songeditor_s;
int songeditor_ts;
int songeditor_selection;
int songeditor_pattern;

void songeditor_init() {
  songeditor_s = MAX_SLIDE;
  songeditor_ts = MAX_SLIDE;
  songeditor_selection = 0;
  songeditor_pattern = 0;
}

void songeditor_enter(int forward) {
  songeditor_ts = 0;
}

void songeditor_leave(int forward) {
  songeditor_ts = forward ? MIN_SLIDE : MAX_SLIDE;
}

void songeditor_input(int button, int repeat) {
  if (songeditor_selection == 0) {

    if (button == LEFT_BUTTON && repeat == 0) {
      songeditor_pattern --;
      if (songeditor_pattern < 0)
        songeditor_pattern = 3;
    }

    if (button == RIGHT_BUTTON && repeat == 0) {
      songeditor_pattern ++;
      if (songeditor_pattern > 3)
        songeditor_pattern = 0;
    }

    if (button == A_BUTTON && repeat == 0) {
      player_cuedpattern = songeditor_pattern;
    }

  } else if (songeditor_selection == 1) {

    if (button == LEFT_BUTTON) {
      if (_song->tempo > 60) _song->tempo --;
      _song->dirty = 1;
    }

    if (button == RIGHT_BUTTON) {
      if (_song->tempo < 180) _song->tempo ++;
      _song->dirty = 1;
    }

  }

  if (button == B_BUTTON && repeat == 0) {
    switchScreen(SCREEN_HOME, false);
  }

  if (button == UP_BUTTON && repeat == 0) {
    if (songeditor_selection > 0) songeditor_selection --;
  }

  if (button == DOWN_BUTTON && repeat == 0) {
    if (songeditor_selection < 1) songeditor_selection ++;
  }
}

void songeditor_render() {
  char buf[10];

  if (songeditor_s < songeditor_ts) songeditor_s ++;
  if (songeditor_s > songeditor_ts) songeditor_s --;

  if (songeditor_s > MIN_SLIDE && songeditor_s < MAX_SLIDE) {
    int bx = slidePosition(songeditor_s);

    _arduboy->fillRect(bx, 0, 128, 64, 0);
    _arduboy->setCursor(bx + 20, 20);
    _arduboy->print("SONG EDITOR");

    _arduboy->setCursor(bx + 20, 35);
    if (songeditor_selection == 0) {
      _arduboy->print(">PAT");
    } else {
      _arduboy->print(" PAT");
    }

    for(int j=0; j<4; j++) {

      int x = bx + 60 + 20 * j;

      if ((j == player_pattern && (player_step & 3) == 0) || j == player_cuedpattern) {
        _arduboy->drawRect(x-4, 31, 16, 16, 1);
      }

      if (j == songeditor_pattern) {
        _arduboy->drawRect(x-2, 33, 12, 12, 1);
      }

      _arduboy->setCursor(x, 35);

      sprintf(buf, "%d", j);
      _arduboy->print(buf);
    }

    _arduboy->setCursor(bx + 20, 50);
    if (songeditor_selection == 1) {
      _arduboy->print(">BPM");
    } else {
      _arduboy->print(" BPM");
    }

    _arduboy->setCursor(bx + 60, 50);
    sprintf(buf, "%d", _song->tempo);
    _arduboy->print(buf);
  }
}

void songeditor_update() {
}
