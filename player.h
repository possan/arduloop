#ifndef _PLAYER_H_
#define _PLAYER_H_

extern volatile unsigned long player_substepinterval1;
extern volatile unsigned long player_substepinterval2;
extern volatile unsigned long player_substep;
extern volatile int player_step;
extern volatile int player_pattern;
extern volatile int player_cuedpattern;
extern volatile int player_playing;

extern volatile int pwm_position;

extern uint8_t sine_wave[245];

extern volatile int phase;
extern volatile int phase_delta;

extern volatile int gen_state;
extern volatile long gen_counter;

extern volatile long pitch_value;
extern volatile long pitch_target;
extern volatile long pitch_delta;

extern volatile int volume;

extern void startTimers();

#endif
