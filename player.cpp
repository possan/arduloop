#include "global.h"
#include "song.h"

#include "pins_arduino.h" // Arduino pre-1.0 needs this

volatile unsigned long player_substep;
volatile unsigned long player_substepinterval1;
volatile unsigned long player_substepinterval2;
volatile int player_step;
volatile int player_pattern;
volatile int player_cuedpattern;
volatile int player_playing;

#define PIN_SYNC_OUT A5

volatile int pwm_position = 0;

volatile unsigned long lfo_offset = 0;

// int configvalue1 = 24;
// int configvalue2 = 8;
// int configvalue3 = 16;
// int configvalue4 = 4;
// int configvalue5 = 0;
// int configvalue6 = 16;

volatile int phase = 0;
volatile int phase_delta = 16;

volatile int gen_state = 0;
volatile long gen_counter = 0;

volatile int shape = 0;
volatile int pw = 0;
volatile int pwdelta = 0;

volatile long pitch_value = 0;
volatile long pitch_target = 0;
volatile long pitch_delta = 0;

volatile int volume = 0;

uint8_t noise_table[256] = { 0, };

uint8_t  sine_wave[256] = {
  0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
  0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
  0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
  0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
  0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
  0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
  0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
  0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
  0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
  0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
  0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
  0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
  0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
  0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
  0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
  0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
  0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
  0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
  0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
  0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
  0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
  0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
  0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
  0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
  0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
  0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
  0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
  0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
  0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
};




volatile byte *_tunes_timer3_pin_port;
volatile byte _tunes_timer3_pin_mask;
volatile byte *_tunes_timer3_pin2_port;
volatile byte _tunes_timer3_pin2_mask;
volatile byte *_tunes_timer3_pin3_port;
volatile byte _tunes_timer3_pin3_mask;




volatile int sync_output = 0;

// TIMER 3
ISR(TIMER3_COMPA_vect) {
  int output = 0;

  if (gen_state == 1) {

    if (gen_counter > 0) {
      phase += (pitch_value >> 14);
      while (phase >= 256) {
        phase -= 256;
      }

      if (shape == 0) {
        output = (phase > pw) ? 2 : 0;
      } else if (shape == 1) {
        output = sine_wave[ phase & 255 ] / 90;
      } else {
        output = noise_table[ phase & 255 ] / 90;
      }

      pitch_value += pitch_delta;
      gen_counter --;
    } else  {
      gen_state = 2;
    }
  }

  if (output && volume > 1) {
    *_tunes_timer3_pin_port |= _tunes_timer3_pin_mask;
  } else {
    *_tunes_timer3_pin_port &= ~_tunes_timer3_pin_mask;
  }

  if (output && volume > 0) {
    *_tunes_timer3_pin2_port &= ~_tunes_timer3_pin2_mask;
  } else {
    *_tunes_timer3_pin2_port |= _tunes_timer3_pin2_mask;
  }

  if (sync_output > 0) {
    *_tunes_timer3_pin3_port |= _tunes_timer3_pin3_mask;
    sync_output --;
  } else {
    *_tunes_timer3_pin3_port &= ~_tunes_timer3_pin3_mask;
  }

  struct PATTERN *pat = NULL;
  if (player_pattern != -1) {
    pat = (struct PATTERN *)&_song->patterns[player_pattern];
  }

  if (player_playing) {
    player_substep ++;
    int ssi = (player_step & 1) ? player_substepinterval2 : player_substepinterval1;
    if (player_substep > ssi) {
  
      lfo_offset ++;
  
      
      player_substep = 0;
  
      player_step ++;
  
      int pattern_length = 16;
      if (pat != NULL) {
        pattern_length = pat->numnotes;
      }
  
      if (player_step >= pattern_length) {
        player_step = 0;
  
        // TODO: recalculate bpm -> substep interval
        // 60 bpm = ~1000
        // 120 bpm = ~500
        ssi = (60000L) / (long)_song->tempo;
        player_substepinterval1 = ssi + (_song->shuffle * 50);
        player_substepinterval2 = ssi - (_song->shuffle * 50);
  
        // figure out which pattern...
        if (player_cuedpattern != -1) {
          player_pattern = player_cuedpattern;
          player_cuedpattern = -1;
        }
      }
  
      pat = NULL;
      if (player_pattern != -1) {
        pat = (struct PATTERN *)&_song->patterns[player_pattern];
      }
  
      if (pat != NULL) {
        struct NOTE *note = (struct NOTE *)&pat->notes[player_step];
  
        if (note->instrument != 0) {
          struct INSTRUMENT *instrument = (struct INSTRUMENT *)&_song->instruments[note->instrument - 1];
  
          gen_state = 1;
          gen_counter = 20 * instrument->sustain;
  
          pitch_value = instrument->freq;
          pitch_target = instrument->freqchange;
          pitch_delta = ((pitch_target - pitch_value) * 32768) / gen_counter;
          pitch_value *= 32768;
  
          shape = instrument->shape & 3;
          pw = instrument->pw * 8;
          pwdelta = instrument->pwmotion;
      
          if (pwdelta > 0) {
              pw += lfo_offset * pwdelta / 1;
              pw %= 192;
              pw += 8;
          }  
  
          if ((player_step % 2) == 0) {
            sync_output = 100;
          }
        }
      }
    }
  }
}




void startTimers() {
  player_step = 15;
  player_substep = 0;
  player_pattern = -1;
  player_cuedpattern = 0;
  player_substepinterval1 = 1500;
  player_substepinterval2 = 1500;

  for(int i=0; i<256; i++) {
    noise_table[i] = rand() & 255;
  }

  TCCR3A = 0;
  TCCR3B = 0;
  bitWrite(TCCR3B, WGM32, 1);
  bitWrite(TCCR3B, CS30, 1);

  int pin = PIN_SPEAKER_1;
  pinMode(pin, OUTPUT);
  _tunes_timer3_pin_port = portOutputRegister(digitalPinToPort(pin));
  _tunes_timer3_pin_mask = digitalPinToBitMask(pin);

  pin = PIN_SPEAKER_2;
  pinMode(pin, OUTPUT);
  _tunes_timer3_pin2_port = portOutputRegister(digitalPinToPort(pin));
  _tunes_timer3_pin2_mask = digitalPinToBitMask(pin);

  pin = PIN_SYNC_OUT;
  pinMode(pin, OUTPUT);
  _tunes_timer3_pin3_port = portOutputRegister(digitalPinToPort(pin));
  _tunes_timer3_pin3_mask = digitalPinToBitMask(pin);

  unsigned long ocr;
  unsigned int frequency2;
  byte prescalar_bits;

  ocr = F_CPU / 4000;
  prescalar_bits = 0b001;

  TCCR3B = (TCCR3B & 0b11111000) | prescalar_bits;
  cli();
  OCR3A = ocr;
  sei();
  bitWrite(TIMSK3, OCIE3A, 1);
}
