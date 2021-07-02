#ifndef _SONG_H_
#define _SONG_H_

#define MAX_NOTES 16
#define MAX_INSTRUMENTS 8
#define MAX_PATTERNS 4

struct NOTE {
  unsigned char note;
  unsigned char instrument;
};

struct INSTRUMENT {
  unsigned int freq;
  unsigned int freqchange;
  unsigned char pw;
  signed char pwchange;
  unsigned char sustain;
  unsigned char shape;
};

struct PATTERN {
  struct NOTE notes[MAX_NOTES];
  unsigned char numnotes;
};

struct SONG {
  int songlength;
  struct PATTERN patterns[MAX_PATTERNS];
  struct INSTRUMENT instruments[MAX_INSTRUMENTS];
  unsigned char tempo;
  unsigned char dirty;
};

extern struct SONG *_song;

void clearSong();
void loadSong();
void saveSong();

#endif
