#ifndef _SONGEDITOR_H_
#define _SONGEDITOR_H_

extern void songeditor_init();
extern void songeditor_enter(int forward);
extern void songeditor_leave(int forward);
extern void songeditor_input(int button, int repeat);
extern void songeditor_render();
extern void songeditor_update();

#endif
