#include "global.h"
#include "song.h"

#define EEPROM_SONG_START 10

struct SONG song;
struct SONG *_song = &song;

void clearSong() {
  song.tempo = 120;
  song.shuffle = 0;

  for(int j=0; j<MAX_PATTERNS; j++) {
    struct PATTERN *pat = (struct PATTERN *)&song.patterns[j];
    pat->numnotes = 16;
    for(int i=0; i<MAX_NOTES; i++) {
      struct NOTE *note = (struct NOTE *)&pat->notes[i];
      note->note = 0;
      note->instrument = 0;
    }
  }

  for(int j=0; j<MAX_INSTRUMENTS; j++) {
    struct INSTRUMENT *in = (struct INSTRUMENT *)&song.instruments[j];
    in->freq = 12;
    in->freqchange = 12;
    in->pw = 16;
    in->pwmotion = 0;
    in->sustain = 32;
    in->shape = 0;
  }
}

void loadSong() {
  unsigned char *songptr = (unsigned char *)&song;
  int songsize = sizeof(struct SONG);

  unsigned char crc = EEPROM.read(EEPROM_SONG_START + 0);
  if (crc == (songsize & 255)) {
    for(int i=0; i<songsize; i++) {
      songptr[i] = EEPROM.read(EEPROM_SONG_START + 1 + i);
    }
  }

  song.dirty = 0;
}

void saveSong() {
  song.dirty = 0;

  unsigned char *songptr = (unsigned char *)&song;
  int songsize = sizeof(struct SONG);

  unsigned char crc = songsize & 255;
  EEPROM.write(EEPROM_SONG_START, crc);
  for(int i=0; i<songsize; i++) {
    EEPROM.write(EEPROM_SONG_START + 1 + i, songptr[i]);
  }
}
