#ifndef _TRACKEDITOR_H_
#define _TRACKEDITOR_H_

extern void trackeditor_init();
extern void trackeditor_enter(int forward);
extern void trackeditor_leave(int forward);
extern void trackeditor_input(int button, int repeat);
extern void trackeditor_render();
extern void trackeditor_update();

#endif
