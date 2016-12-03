#include "global.h"
#include "instrumenteditor.h"
#include "songeditor.h"
#include "trackeditor.h"
#include "instrumenteditor.h"
#include "player.h"
#include "song.h"

int instrumenteditor_s;
int instrumenteditor_ts;
int instrumenteditor_selection;
int instrumenteditor_instrument;

void instrumenteditor_init() {
  instrumenteditor_s = MAX_SLIDE;
  instrumenteditor_ts = MAX_SLIDE;
  instrumenteditor_selection = 0;
  instrumenteditor_instrument = 0;
}

void instrumenteditor_enter(int forward) {
  instrumenteditor_ts = 0;
  instrumenteditor_selection = 0;
}

void instrumenteditor_leave(int forward) {
  instrumenteditor_ts = forward ? MIN_SLIDE : MAX_SLIDE;
}

void instrumenteditor_input(int button, int repeat) {
  if (button == B_BUTTON && repeat == 0) {
    switchScreen(SCREEN_HOME, false);
  }

  if (instrumenteditor_selection == 0) {
    // header

    // if (button == LEFT_BUTTON && repeat == 0) {
    //   switchScreen(SCREEN_MAINMENU, false);
    //   saveSong();
    // }

    if (button == LEFT_BUTTON && repeat == 0) {
      instrumenteditor_instrument --;
      if (instrumenteditor_instrument < 0) {
        instrumenteditor_instrument = MAX_INSTRUMENTS - 1;
      }
    }

    if (button == RIGHT_BUTTON && repeat == 0) {
      instrumenteditor_instrument ++;
      if (instrumenteditor_instrument >= MAX_INSTRUMENTS) {
        instrumenteditor_instrument = 0;
      }
    }
  }

  struct INSTRUMENT *ins = (struct INSTRUMENT *)&_song->instruments[instrumenteditor_instrument];

  if (instrumenteditor_selection >= 1 ) {
    // setting

    if (button == LEFT_BUTTON) {
      if (instrumenteditor_selection == 1) {
        if (ins->freq > 0) ins->freq --;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 2) {
        if (ins->freqchange > 0) ins->freqchange --;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 3) {
        if (ins->sustain > 0) ins->sustain --;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 4) {
        if (ins->shape > 0) ins->shape --;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 5) {
        if (ins->pw > 0) ins->pw --; else ins->pw = 31;
        _song->dirty = 1;
      }
    }

    if (button == RIGHT_BUTTON) {
      if (instrumenteditor_selection == 1) {
        if (ins->freq < 31) ins->freq ++;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 2) {
        if (ins->freqchange < 31) ins->freqchange ++;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 3) {
        if (ins->sustain < 31) ins->sustain ++;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 4) {
        if (ins->shape < 2) ins->shape ++;
        _song->dirty = 1;
      }

      if (instrumenteditor_selection == 5) {
        if (ins->pw < 31) ins->pw ++; else ins->pw = 0;
        _song->dirty = 1;
      }
    }
  }


  if (button == UP_BUTTON && repeat == 0) {
    instrumenteditor_selection --;
    if (instrumenteditor_selection < 0) {
      instrumenteditor_selection = 7;
    }
  }

  if (button == DOWN_BUTTON && repeat == 0) {
    instrumenteditor_selection ++;
    if (instrumenteditor_selection > 7) {
      instrumenteditor_selection = 0;
    }
  }

}

void instrumenteditor_render() {
  if (instrumenteditor_s < instrumenteditor_ts) instrumenteditor_s ++;
  if (instrumenteditor_s > instrumenteditor_ts) instrumenteditor_s --;

  if (instrumenteditor_s > MIN_SLIDE && instrumenteditor_s < MAX_SLIDE) {
    int bx = slidePosition(instrumenteditor_s);

    struct INSTRUMENT *ins = (struct INSTRUMENT *)&_song->instruments[instrumenteditor_instrument];

    _arduboy->fillRect(bx, 0, 128, 64, 0);
    char buf[30];
    if (instrumenteditor_selection == 0) {
      sprintf(buf, ">INSTRUMENT %d", instrumenteditor_instrument);
    } else {
      sprintf(buf, " INSTRUMENT %d", instrumenteditor_instrument);
    }

    _arduboy->setCursor(bx, 0);
    _arduboy->print(buf);

    drawbar(bx, "STA", ins->freq, 14, (instrumenteditor_selection == 1));
    drawbar(bx, "END", ins->freqchange, 24, (instrumenteditor_selection == 2));
    drawbar(bx, "SUS", ins->sustain, 34, (instrumenteditor_selection == 3));
    drawbar(bx, "SHP", ins->shape * 10, 44, (instrumenteditor_selection == 4));
    drawbar(bx, "PW",  ins->pw, 54, (instrumenteditor_selection == 5));
  }
}

void instrumenteditor_update() {
}
