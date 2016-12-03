#ifndef _HOME_H_
#define _HOME_H_

extern void home_init();
extern void home_enter(int forward);
extern void home_leave(int forward);
extern void home_input(int button, int repeat);
extern void home_render();
extern void home_update();

#endif
