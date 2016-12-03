#ifndef _INSTRUMENTEDITOR_H_
#define _INSTRUMENTEDITOR_H_

extern void instrumenteditor_init();
extern void instrumenteditor_enter(int forward);
extern void instrumenteditor_leave(int forward);
extern void instrumenteditor_input(int button, int repeat);
extern void instrumenteditor_render();
extern void instrumenteditor_update();

#endif
