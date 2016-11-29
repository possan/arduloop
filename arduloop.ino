#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include "Arduboy2.h"

Arduboy2 arduboy;

#define EEPROM_SONG_START 10

#include "pins_arduino.h" // Arduino pre-1.0 needs this

struct NOTE {
  int freq; 
  int freqchange;
  unsigned char pw;
  signed char pwchange;
  unsigned char sustain;
  unsigned char shape;
};


struct SONG {
  struct NOTE notes[16];
  unsigned char numnotes;
  unsigned char tempo;
};

struct SONG song;



void clearSong() {
  song.tempo = 120;
  song.numnotes = 4;
  for(int i=0; i<16; i++) {
    struct NOTE *n = (struct NOTE *)&song.notes[i]; 
    n->freq = 0; 
    n->freqchange = 0; 
    n->pw = 128;
    n->pwchange = 0; 
    n->sustain = 64; 
    n->shape = 0; 
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

}

void saveSong() {
  unsigned char *songptr = (unsigned char *)&song;
  int songsize = sizeof(struct SONG);

  unsigned char crc = songsize & 255;
  EEPROM.write(EEPROM_SONG_START, crc);
  for(int i=0; i<songsize; i++) {
    EEPROM.write(EEPROM_SONG_START + 1 + i, songptr[i]);
  }
}




#define REPEAT_INTERVAL 100

volatile int pwm_position = 0;

int configvalue1 = 24;
int configvalue2 = 8;
int configvalue3 = 16;
int configvalue4 = 4;
int configvalue5 = 0;
int configvalue6 = 16;

volatile int phase = 0;
volatile int phase_delta = 16;

volatile int gen_state = 0;
volatile long gen_counter = 0;

volatile long pitch_value = 0;
volatile long pitch_target = 0;
volatile long pitch_delta = 0;

int last_down = 0;
long last_down_rep = 0;

int selection = 0;

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


#define SHIFT 3

struct BALL {
  int x, y, dx, dy;
  int s;
};

#define NUMBALLS 8

struct BALL balls[NUMBALLS];






volatile byte *_tunes_timer3_pin_port;
volatile byte _tunes_timer3_pin_mask;
volatile byte *_tunes_timer3_pin2_port;
volatile byte _tunes_timer3_pin2_mask;





// TIMER 3
ISR(TIMER3_COMPA_vect)
{
  // Timer 3 is the one assigned first, so we keep it running always
  // and use it to time score waits, whether or not it is playing a note.
  // rduboyTunes::soundOutput();

  int output = 0;


  if (gen_state == 0) {

    //
    // start it!
    //

    gen_state = 1;
    gen_counter = 20 * configvalue3;

    pitch_value = 1 + ((configvalue1 * configvalue1) >> 2);
    pitch_target = 1 + ((configvalue2 * configvalue2) >> 2);

    pitch_delta = ((pitch_target - pitch_value) * 32768) / gen_counter; 
    pitch_value *= 32768;

  } else if (gen_state == 1) {

    //
    // interpolate
    //

    int shape = (configvalue5 % 3);
    int pw = configvalue6 * 8;
 
    if (gen_counter > 0) {
      phase += (pitch_value >> 14);
      while (phase >= 256) {
        phase -= 256;
      }

      if (shape == 0) {
        // square
        output = (phase > pw);
      } else if (shape == 1) {
        // sine
        output = sine_wave[ phase & 255 ] > pw;
      } else {
        // noise
        output = noise_table[ phase & 255 ] > pw;
      }
      
      pitch_value += pitch_delta;
      gen_counter --;
    } else  {
      gen_counter = 20 * configvalue4;
      gen_state = 2;
    }

  } else if (gen_state == 2) {

    //
    // pausing
    //

    if (gen_counter > 0) {
      // do nothing
      gen_counter --;
    } else {
      // restart    
      gen_counter = 0;  
      gen_state = 0;
    }

  }


  /*
  
  phase += phase_delta;
  while (phase > 32767) {
    phase -= 32768;
  }
  while (phase < 0) {
    phase += 32768;
  }

  int p = phase >> 7;
  // p = sine_wave[p & 255];
  
  sound_position ++; // = p;
  while (sound_position > 1024) {
    sound_position -= 1024;
  }
  while (sound_position < 0) {
    sound_position += 1024;
  }
  */
  //  int s = (sound_position & 31) > phase_delta; // 32767;// sound_buffer[sound_position];

  if (output && volume) {
   *_tunes_timer3_pin_port |= _tunes_timer3_pin_mask;
  } else {
   *_tunes_timer3_pin_port &= ~_tunes_timer3_pin_mask;
  }

  if (output && volume) {
   *_tunes_timer3_pin2_port &= ~_tunes_timer3_pin2_mask;
  } else {
   *_tunes_timer3_pin2_port |= _tunes_timer3_pin2_mask;
  }

/*
  sound_position += sound_speed;
  if (sound_position < 0) {
    sound_position = NUMSAMPLES - 1;
  }
  if (sound_position >= NUMSAMPLES) {
    sound_position = 0;
  }
  */
}








void startTimer() {

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

  /*
  cli();
  OCR3A   = 0x0342;
  sei();
  */

  unsigned long ocr;
  unsigned int frequency2;
  byte prescalar_bits;

  ocr = F_CPU / 4000;
  //  / frequency2 - 1;
  prescalar_bits = 0b001;
  /*if (ocr > 0xffff) {
    ocr = F_CPU / frequency2 / 64 - 1;
    prescalar_bits = 0b011;
    } */


  TCCR3B = (TCCR3B & 0b11111000) | prescalar_bits;
  cli();
  OCR3A = ocr;
  sei();
  bitWrite(TIMSK3, OCIE3A, 1);  
}


void waitStart() {
  arduboy.clear();
  arduboy.setCursor(8, 24);
  arduboy.print("DRUMS");
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

  for(int i=0; i<256; i++) {
    noise_table[i] = rand() & 255;
  }






  for (int i = 0; i < NUMBALLS; i++) {
    BALL *b = (BALL *)&balls[i];
    b->x = rand() % 127;
    b->y = rand() % 60;
    int s = 1 + (rand() % 6);
    b->s = i % 2;
    b->x <<= SHIFT;
    b->y <<= SHIFT;
    b->dx = s * (-1 + ((rand() % 2) * 3));
    b->dy = s * (-1 + ((rand() % 2) * 3));
  }

  clearSong();
  loadSong();

  waitStart();
  startTimer();
}

int temp1;

void drawbar(char *label, int value, int y, int selected) {
  
  arduboy.setCursor(0, y);
  arduboy.print(label);

  if (selected) {
    arduboy.fillRect(40, y, 64 + 4, 8, 1);
    arduboy.fillRect(41, y+1, 64 + 2, 6, 0);
  }

  if (value < 1) value = 1;
  if (value > 31) value = 31;
  arduboy.fillRect(42, y+2, value * 2, 4, 1);
}

void loop()
{
  if (arduboy.nextFrame()) {
    arduboy.clear();

    // arduboy.setCursor(0, rand() % 100);
    // arduboy.print("Hello World!");

    int maxwidth = 128 << SHIFT;
    int maxheight = 64 << SHIFT;
    for (int i = 0; i < NUMBALLS; i++) {
      BALL *b = (BALL *)&balls[i];
      int x = b->x >> SHIFT;
      int y = b->y >> SHIFT;
      arduboy.drawPixel(x, y, 1);
      if (b->s == 1) {
        arduboy.drawPixel(x - 1, y, 1);
        arduboy.drawPixel(x, y - 1, 1);
        arduboy.drawPixel(x + 1, y, 1);
        arduboy.drawPixel(x, y + 1, 1);
      }


      b->x += b->dx;
      b->y += b->dy;
      if (b->x > maxwidth) {
        b->dx = -abs(b->dx);
      }
      if (b->y > maxheight) {
        b->dy = -abs(b->dy);
      }
      if (b->x < 0) {
        b->dx = abs(b->dx);
      }
      if (b->y < 0) {
        b->dy = abs(b->dy);
      }
    }

    // arduboy.drawPixel(sound_position>>4, 32, 1);

    drawbar("STA",  configvalue1, 0, (selection == 0));
    drawbar("END",  configvalue2, 8, (selection == 1));
    drawbar("SUS", configvalue3, 20, (selection == 2));
    drawbar("DEL", configvalue4, 28, (selection == 3));
    drawbar("SHP", configvalue5, 40, (selection == 4));
    drawbar("PW",  configvalue6, 48, (selection == 5));
    
    arduboy.display();
  }

  long T = millis();
  long d = T - last_down_rep;
  
  if (arduboy.pressed(LEFT_BUTTON)) {
    volume = 1;
    if (d > REPEAT_INTERVAL) {
      last_down_rep = T;
      if (selection == 0) {
        configvalue1 --;
        if (configvalue1 < 0) configvalue1 = 0;
      }
      if (selection == 1) {
        configvalue2 --;
        if (configvalue2 < 0) configvalue2 = 0;
      }
      if (selection == 2) {
        configvalue3 --;
        if (configvalue3 < 0) configvalue3 = 0;
      }
      if (selection == 3) {
        configvalue4 --;
        if (configvalue4 < 0) configvalue4 = 0;
      }
      if (selection == 4) {
        configvalue5 --;
        if (configvalue5 < 0) configvalue5 = 0;
      }
      if (selection == 5) {
        configvalue6 --;
        if (configvalue6 < 0) configvalue6 = 0;
      }
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    volume = 1;
    if (d > REPEAT_INTERVAL) {
      last_down_rep = T;
      if (selection == 0) {
        configvalue1 ++;
        if (configvalue1 > 31) configvalue1 = 31;
      }
      if (selection == 1) {
        configvalue2 ++;
        if (configvalue2 > 31) configvalue2 = 31;
      }
      if (selection == 2) {
        configvalue3 ++;
        if (configvalue3 > 31) configvalue3 = 31;
      }
      if (selection == 3) {
        configvalue4 ++;
        if (configvalue4 > 31) configvalue4 = 31;
      }
      if (selection == 4) {
        configvalue5 ++;
        if (configvalue5 > 31) configvalue5 = 31;
      }
      if (selection == 5) {
        configvalue6 ++;
        if (configvalue6 > 31) configvalue6 = 31;
      }
      last_down = 1;
    }
  } else if (arduboy.pressed(UP_BUTTON)) {
    volume = 1;
    if (!last_down) {
      selection --;
      if (selection < 0) {
        selection = 0;
      }
      last_down = 1;
    }
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    volume = 1;
    if (!last_down) {
      selection ++;
      if (selection >= 5) {
        selection = 5;
      }
      last_down = 1;
    }
  } else {
    last_down = 0;
  }
  
  
  if (arduboy.pressed(A_BUTTON)) {
    volume = 1;
  } else if (arduboy.pressed(B_BUTTON)) {
    volume = 0u;
  }

  // update audio
}


