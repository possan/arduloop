#include "global.h"
#include "trackeditor.h"
#include "song.h"
#include "player.h"

int trackeditor_s;
int trackeditor_ts;
int trackeditor_selection;
int trackeditor_pattern;
int trackeditor_step;

void trackeditor_init() {
  trackeditor_s = MAX_SLIDE;
  trackeditor_ts = MAX_SLIDE;
  trackeditor_selection = 0;
  trackeditor_pattern = 0;
  trackeditor_step = 0;
}

void trackeditor_enter(int forward) {
  trackeditor_ts = 0;

  trackeditor_selection = 0;
  trackeditor_step = 0;
}

void trackeditor_leave(int forward) {
  trackeditor_ts = forward ? MIN_SLIDE : MAX_SLIDE;
}

void trackeditor_input(int button, int repeat) {
  if (button == B_BUTTON && repeat == 0) {
    switchScreen(SCREEN_HOME, false);
  }

  if (button == DOWN_BUTTON) {
    trackeditor_selection ++;
    if (trackeditor_selection >= 17) {
      trackeditor_selection = 0;
    }
  }

  if (button == UP_BUTTON) {
    trackeditor_selection --;
  }

  if (trackeditor_selection == 0) {
    if (button == LEFT_BUTTON && repeat == 0) {
      trackeditor_pattern --;
      if (trackeditor_pattern < 0) {
        trackeditor_pattern = MAX_PATTERNS - 1;
      }
    }

    if (button == RIGHT_BUTTON && repeat == 0) {
      trackeditor_pattern ++;
      if (trackeditor_pattern >= MAX_PATTERNS) {
        trackeditor_pattern = 0;
      }
    }
  } else if (trackeditor_selection < 17) {
    struct PATTERN *pat = (struct PATTERN *)&_song->patterns[trackeditor_pattern];
    if (button == LEFT_BUTTON && repeat == 0) {
      struct NOTE *note = (struct NOTE *)&pat->notes[trackeditor_selection - 1];
      if (note->instrument > 0) {
        note->instrument --;
      }
      _song->dirty = 1;
    }
    if (button == RIGHT_BUTTON && repeat == 0) {
      struct NOTE *note = (struct NOTE *)&pat->notes[trackeditor_selection - 1];
      if (note->instrument < MAX_INSTRUMENTS) {
        note->instrument ++;
      }
      _song->dirty = 1;
    }
  }
}

void trackeditor_render() {
  if (trackeditor_s < trackeditor_ts) trackeditor_s ++;
  if (trackeditor_s > trackeditor_ts) trackeditor_s --;

  if (trackeditor_s > MIN_SLIDE && trackeditor_s < MAX_SLIDE) {
    int bx = slidePosition(trackeditor_s);

    _arduboy->fillRect(bx, 0, 128, 64, 0);
    _arduboy->setCursor(bx + 10, 5);

    char buf[5];
    if (trackeditor_selection == 0) {
      sprintf(buf, ">P%d", trackeditor_pattern);
    } else {
      sprintf(buf, " P%d", trackeditor_pattern);
    }
    _arduboy->print(buf);

    struct PATTERN *pat = (struct PATTERN *)&_song->patterns[trackeditor_pattern];

    for(int r=0; r<4; r++) {
      for( int k=0; k<4; k++) {
        int n = r * 4 + k;
        int x = 64 + bx + k * 14;
        int y = 10 + r * 14;
        _arduboy->setCursor(x, y);
        struct NOTE *note = (struct NOTE *)&pat->notes[n];

        if (player_step == n && player_pattern == trackeditor_pattern) {
          _arduboy->drawRect(x-3, y-3, 14, 14, 1);
        }

        if (trackeditor_selection == n + 1) {
          _arduboy->drawRect(x-2, y-2, 12, 12, 1);
        }

        if (note->instrument != 0) {
          sprintf(buf, "%d", note->instrument);
          _arduboy->print(buf);
        } else {
          _arduboy->print("-");
        }
      }
    }
  }
}

void trackeditor_update() {
}
